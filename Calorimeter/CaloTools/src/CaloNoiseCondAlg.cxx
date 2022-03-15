/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloNoiseCondAlg.h" 
#include <forward_list>
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"
#include "CaloIdentifier/CaloCell_ID.h"

StatusCode CaloNoiseCondAlg::initialize() {

  unsigned nFolders=0;
  if (!m_caloNoiseKey.key().empty()) {
    ATH_CHECK(m_caloNoiseKey.initialize());
    nFolders++;
  }
  if (!m_larNoiseKey.key().empty()) {
    ATH_CHECK(m_larNoiseKey.initialize());
    nFolders++;
  }
  if (!m_tileNoiseKey.key().empty()) {
    ATH_CHECK(m_tileNoiseKey.initialize());
    nFolders++;
  }

  if (nFolders==0) {
    ATH_MSG_ERROR("No noise DB folder found, LArNoiseFolder, TileNoiseFolder and CaloNoiseFolder properties are all empty!");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_hvCorrKey.initialize(m_useHVCorr) );

  const bool doLumiFolderInit = m_lumi0 < 0;
  if (m_lumiFolderKey.initialize(doLumiFolderInit).isFailure()) {
      ATH_MSG_ERROR("Luminosity set to < 0 but failed to initialize LumiFolder");
  }
 
  ATH_CHECK( m_cablingKey.initialize() );

  const std::string& noiseKey=m_outputKey.key();
  if(noiseKey=="electronicNoise") {
    ATH_MSG_INFO("Will compute electronic noise");
    m_noiseType=CaloNoise::ELEC;
  }
  else if (noiseKey=="pileupNoise") {
    ATH_MSG_INFO("Will compute pileup noise");
    m_noiseType=CaloNoise::PILEUP;
  }
  else if (noiseKey=="totalNoise") {
    m_noiseType=CaloNoise::TOTAL;
    ATH_MSG_INFO("Will compute total (electronic + pileup)  noise");
  }
  else {
    ATH_MSG_ERROR("Unexpected noise key given: " <<  noiseKey << ". Expeced 'electronicNoise' or 'pileupNoise' or 'totalNoise'.");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_outputKey.initialize());

  ATH_CHECK(detStore()->retrieve(m_caloCellID,"CaloCell_ID"));

  m_hashRange=std::make_unique<CaloNoiseHashRanges>(m_caloCellID);

  return StatusCode::SUCCESS;
}


StatusCode CaloNoiseCondAlg::execute(const EventContext& ctx) const {

  //Set up write handle
  SG::WriteCondHandle<CaloNoise> writeHandle{m_outputKey,ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  writeHandle.addDependency(cablingHdl);
  ATH_MSG_DEBUG("Range of LArCabling " << cablingHdl.getRange() << ", intersection:" << writeHandle.getRange());
  //Obtain AttrListsCollections for all possible folders (LAr,Tile,Calo) 
  std::vector<const CondAttrListCollection*> attrListNoise;

  if (!m_larNoiseKey.key().empty()) {
    //Separete LAr/Tile folder
    SG::ReadCondHandle<CondAttrListCollection> larNoiseHdl{m_larNoiseKey,ctx};
    attrListNoise.push_back(*larNoiseHdl);
    writeHandle.addDependency(larNoiseHdl);
    ATH_MSG_DEBUG("Range of LArNoise " << larNoiseHdl.getRange() << ", intersection:" << writeHandle.getRange());
  }

  if (!m_tileNoiseKey.key().empty()) {
    SG::ReadCondHandle<CondAttrListCollection> tileNoiseHdl{m_tileNoiseKey,ctx};
    attrListNoise.push_back(*tileNoiseHdl);
     writeHandle.addDependency(tileNoiseHdl);
     ATH_MSG_DEBUG("Range of TileNoise " << tileNoiseHdl.getRange() << ", intersection:" <<  writeHandle.getRange());
  }

  if (!m_caloNoiseKey.key().empty()) {
    SG::ReadCondHandle<CondAttrListCollection> caloNoiseHdl{m_caloNoiseKey,ctx};
    attrListNoise.push_back(*caloNoiseHdl);
    writeHandle.addDependency(caloNoiseHdl);
    ATH_MSG_DEBUG("Range of CaloNoise " << caloNoiseHdl.getRange() << ", intersection:" << writeHandle.getRange());
  }
  
  //Get noise-blobs out of all COOL-channels in all COOL Folders we know about:
  std::forward_list<std::pair<unsigned, const coral::Blob&> > blobList;  
  for (const auto& attrListColl : attrListNoise) {
    for (const auto& coll : *attrListColl) {
      ATH_MSG_DEBUG("Working on channel " << coll.first);
      const coral::Blob& blob = (coll.second)["CaloCondBlob16M"].data<coral::Blob>();
      if (blob.size()<1) {
	ATH_MSG_DEBUG("Empty blob in channel " << coll.first << ". Ignored.");
	continue;
      }
      blobList.emplace_front(coll.first, blob);
    }
  }

  //Get LAr HVScale Corr (if requested)
  const ILArHVScaleCorr* larHVCorr=nullptr;
  if (m_useHVCorr) {
    SG::ReadCondHandle<ILArHVScaleCorr> larHVCorrHdl{m_hvCorrKey,ctx};
    larHVCorr=*larHVCorrHdl;
     writeHandle.addDependency(larHVCorrHdl);
     ATH_MSG_DEBUG("Range of LArHVScale " << larHVCorrHdl.getRange() << ", intersection:" << writeHandle.getRange());
  }

  //Get Luminosity:
  float lumi=m_lumi0;
  if (m_lumi0<0) {    
    SG::ReadCondHandle<CondAttrListCollection> lumiHdl{m_lumiFolderKey,ctx};
    const CondAttrListCollection* lumiAttrListColl=*lumiHdl;
    writeHandle.addDependency(lumiHdl);
    ATH_MSG_DEBUG("Range of Luminosity " << lumiHdl.getRange() << ", intersection:" << writeHandle.getRange() );
    const coral::AttributeList& attrList = lumiAttrListColl->attributeList(0); //Get lumi from COOL channel 0
    if (attrList["LBAvInstLumi"].isNull()) {
      ATH_MSG_WARNING( " NULL Luminosity information in database ... set it to 0 "  );
      lumi = 0.;
    } else {
      lumi = attrList["LBAvInstLumi"].data<float>() *1e-3;  // luminosity (from 10**30 units in db to 10*33 units)
    }
    if (std::isnan(lumi)) {
      ATH_MSG_WARNING( " Luminosity is not a number.. " << m_lumi0 << "  ... set it to 0 "  );
      lumi=0.;
    }
  }//end if m_lumi0<0 


  const size_t maxCells=m_caloCellID->calo_cell_hash_max();
  //Create the CaloNoise CDO:
  const size_t nLArCells=m_hashRange->maxLArCells();
  const size_t nTileCells=m_hashRange->maxTileCells();
  std::unique_ptr<CaloNoise> caloNoiseObj=std::make_unique<CaloNoise>(nLArCells,3,
								      nTileCells,4,
								      m_caloCellID,m_noiseType);

  //Counters for crosschecks
  std::array<unsigned,4> cellsPerGain{0,0,0,0};
  unsigned nBlobs=0;
 
  //Loop over the list of blobs we got:
  for (auto& blobPair : blobList) {
    nBlobs++;
    const CaloNoiseHashRanges::SYSTEM sys=static_cast<CaloNoiseHashRanges::SYSTEM>(blobPair.first);
    const IdentifierHash offset=m_hashRange->hashOffsets().at(sys);
    std::unique_ptr<const CaloCondBlobFlt> blob(CaloCondBlobFlt::getInstance(blobPair.second));
    if (blob->getObjVersion()!=1) {
      ATH_MSG_ERROR("Unexpected blob object version in COOL channel " << blobPair.first 
		    << ". Found " << blob->getObjVersion() << ", expected 1");
      return StatusCode::FAILURE;
    }
    //Get writeable access to underlying storage (boost::multi_array)
    auto& noise = (sys==CaloNoiseHashRanges::TILE) ? caloNoiseObj->tileStorage() : caloNoiseObj->larStorage();
    
    const unsigned nChansThisblob=blob->getNChans();
    const unsigned nGains=blob->getNGains();
    for (unsigned igain=0;igain<nGains;++igain) {
      for (unsigned i=0;i<nChansThisblob;++i) {
	float hvcorr=1.0;
	if (sys!=CaloNoiseHashRanges::TILE && larHVCorr) {
	  const Identifier id =  m_caloCellID->cell_id(offset+i);
	  const HWIdentifier hwid = cabling->createSignalChannelID(id);
	  hvcorr=larHVCorr->HVScaleCorr(hwid);
	  //hvcorr might be zero in case of problems with the DCS database
	  if (hvcorr<0.01) hvcorr=1.0;
	}					
	const float a=blob->getData(i,igain,0)*hvcorr;
	const float b=blob->getData(i,igain,1);
	++(cellsPerGain[igain]);
	const size_t hash = (sys==CaloNoiseHashRanges::TILE) ? i : i+offset;
	switch (m_noiseType){
	case CaloNoise::ELEC:
	  noise[igain][hash]=a;
	  break;
	case CaloNoise::PILEUP:
	  noise[igain][hash]=b*std::sqrt(lumi);
	  break;
	case CaloNoise::TOTAL:
	  noise[igain][hash]=std::sqrt(a*a + b*b*lumi);
	  break;
	default:
	  break;
	}
      }//end loop over channels
    }//end loop over gains


    // Cache data to calculate effective sigma for tile double-gaussian noise 
    // Matters for Electronic and total noise
    if (sys==CaloNoiseHashRanges::TILE && m_noiseType!=CaloNoise::PILEUP) {
      caloNoiseObj->setTileBlob(blob.release(),lumi);
    }

  }//end loop over blob (COOL channels)

  

  if (nBlobs!=7) {
    ATH_MSG_ERROR("Unexpected number of COOL channels containing noise-blobs. Got " << nBlobs << " expected 7 (6 LAr, 1 Tile)");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Calculated noise for lumi " << lumi << (larHVCorr ? " with " : " without ") << "HV Scale correction");
  for (unsigned igain=0;igain<4;++igain) {
    ATH_MSG_INFO("Gain " << igain << " Nbr of cells: " << cellsPerGain[igain]);
    if (igain<3 && cellsPerGain[igain]!=maxCells) {
      ATH_MSG_ERROR("Expected " << maxCells << " cells for gain " << igain);
    }
  }

  //Create output object  
  ATH_CHECK(writeHandle.record(std::move(caloNoiseObj)));
  ATH_MSG_INFO("recorded new CaloNoise object with key " << writeHandle.key() << " and range " << writeHandle.getRange());
  
  return StatusCode::SUCCESS;
}
