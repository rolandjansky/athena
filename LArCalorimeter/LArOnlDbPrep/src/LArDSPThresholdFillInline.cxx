/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDSPThresholdFillInline.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloConditions/CaloNoise.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include <fstream>

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"

#include "LArCOOLConditions/LArDSPThresholdsFlat.h"
//#include "CoolKernel/types.h"
#include "CoolKernel/StorageType.h"

LArDSPThresholdFillInline::LArDSPThresholdFillInline(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_onlineID(0),
  m_workmode (FIXED)
{

  declareProperty("Fill",m_fill=true);
  declareProperty("NameOfSet",m_nameOfSet);
  declareProperty("Key",m_key="DSPThresholds");
  declareProperty("mode",m_mode="fixed",
		  "Select how to set thresholds. Allowed values are 'fixed','group' and 'noise'");

  //For mode 'fixed'
  declareProperty("tQThreshold",m_tqThrsh=250);
  declareProperty("samplesThreshold",m_samplesThrsh=1000);

  //For mode 'group'
  declareProperty("ThresholdsPerCellGroup",m_cellGroupStr);

  //For mode 'Noise'
  declareProperty("sigmaNoiseSamples",m_sigmaNoiseSamples);
  declareProperty("sigmaNoiseQt",m_sigmaNoiseQt);
  declareProperty("usePileupNoiseSamples",m_usePileupNoiseSamples);
  declareProperty("usePileupNoiseQt",m_usePileupNoiseQt);

  //For channel masking
  declareProperty("MaskBadChannels",m_maskBadChannels=false);
  declareProperty("MaskedtQThreshold",m_maskedtqThrsh=static_cast<float>(0x7fffffff));
  declareProperty("MaskedsamplesThreshold",m_maskedsamplesThrsh=static_cast<float>(0x7fffffff));

  declareProperty("Dump",m_dump=false);
  declareProperty("OutFile",m_outFileName="out.txt");
  //declareProperty("InFile",m_inFileName="");
}

LArDSPThresholdFillInline::~LArDSPThresholdFillInline() {}


StatusCode LArDSPThresholdFillInline::initialize() {

  ATH_MSG_DEBUG ( "start initialize()" );

  ATH_CHECK( detStore()->retrieve(m_onlineID,"LArOnlineID") );
  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_caloMgrKey.initialize() );

  ATH_CHECK(m_bcContKey.initialize(m_maskBadChannels));
  ATH_CHECK(m_bcMask.buildBitMask(m_problemsToMask,msg()));


  if (m_mode.compare("fixed")==0) {
    m_workmode=FIXED;
    ATH_MSG_INFO ( "Will used fixed values defined by jobO " 
                   << name() <<".tQThreshold and "
                   << name() << ".samplesThreshold for DSP thresholds" );
  }
  else if (m_mode.compare("group")==0) {
    m_workmode=GROUP;
    ATH_MSG_INFO ( "Will used cell groups defined in jobO " << name() << ".CellGroup for DSP thresholds" );
    const CaloCell_ID* caloCellID = nullptr;
    ATH_CHECK( detStore()->retrieve(caloCellID,"CaloCell_ID") );

    if (m_thrPerCell.setDefinition(caloCellID,m_cellGroupStr,msg())==false)
      return StatusCode::FAILURE;
    m_thrPerCell.printDef();

    if (m_thrPerCell.getDefaults().size()!=2) {
      ATH_MSG_ERROR ( "Expected 2 values per cell group, got " << m_thrPerCell.getDefaults().size() );
      return StatusCode::FAILURE;
    }
  }
  else if (m_mode.compare("noise")==0) {
    m_workmode=NOISE;

    if(m_usePileupNoiseSamples)
      ATH_MSG_INFO ( "Will used ICaloCellTool::totalNoiseRMS times " << name() << ".sigmaNoiseSamples and" );
    else
      ATH_MSG_INFO ( "Will used ICaloCellTool::elecNoiseRMS times " << name() << ".sigmaNoiseSamples and" );

    if(m_usePileupNoiseQt)
      ATH_MSG_INFO ( "ICaloCellTool::totalNoiseRMS times " << name() << ".sigmaNoiseQt for DSP thresholds" );
    else
      ATH_MSG_INFO ( "ICaloCellTool::elecNoiseRMS times " << name() << ".sigmaNoiseQt for DSP thresholds" );

  }

  ATH_CHECK( m_totalNoiseKey.initialize (m_workmode == NOISE) );
  ATH_CHECK( m_elecNoiseKey.initialize (m_workmode == NOISE) );

  return StatusCode::SUCCESS;
}   

StatusCode LArDSPThresholdFillInline::stop() {

  ATH_MSG_DEBUG ( "start stop()" );

  const EventContext& ctx = Gaudi::Hive::currentContext();

  if (m_fill) {

    ATH_MSG_INFO ( "Filling database" );

    unsigned hashMax=m_onlineID->channelHashMax();
  
    coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
    //cool::RecordSpecification* spec = new cool::RecordSpecification(); 
    spec->extend("tQThr", "blob");
    spec->extend("samplesThr", "blob");
    spec->extend("trigSumThr", "blob");
    spec->extend("Name","string");

    AthenaAttributeList* attr=new AthenaAttributeList(*spec);
  

    (*attr)["Name"].setValue(m_nameOfSet);
    coral::Blob& tQThrBlob = (*attr)["tQThr"].data<coral::Blob>();
    coral::Blob& samplesThrBlob = (*attr)["samplesThr"].data<coral::Blob>();
    coral::Blob& trigSumThrBlob = (*attr)["trigSumThr"].data<coral::Blob>();

    tQThrBlob.resize(hashMax*sizeof(float));
    samplesThrBlob.resize(hashMax*sizeof(float));
    trigSumThrBlob.resize(hashMax*sizeof(float));

    float* ptQThrBlob=static_cast<float*>(tQThrBlob.startingAddress());
    float* psamplesBlob=static_cast<float*>(samplesThrBlob.startingAddress());
    float* ptrigSumBlob=static_cast<float*>(trigSumThrBlob.startingAddress());

    ATH_CHECK( detStore()->record(attr,m_key) );

    SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
    ATH_CHECK(caloMgrHandle.isValid());
    const CaloDetDescrManager *theCaloDDM = *caloMgrHandle;
    ATH_MSG_INFO ( "theCaloDDM retrieved" );

    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl (m_cablingKey, ctx);
    const LArOnOffIdMapping* cabling{*cablingHdl};
    if(!cabling) {
        ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
        return StatusCode::FAILURE;
    }

    const CaloNoise* totalNoise = nullptr;
    const CaloNoise* elecNoise = nullptr;
    if (m_workmode == NOISE) {
      SG::ReadCondHandle<CaloNoise> totalNoiseH (m_totalNoiseKey, ctx);
      totalNoise = totalNoiseH.cptr();
      SG::ReadCondHandle<CaloNoise> elecNoiseH (m_elecNoiseKey, ctx);
      elecNoise = elecNoiseH.cptr();
    }

    //retrieve BadChannel info:
    const LArBadChannelCont* bcCont=nullptr;
    if (m_maskBadChannels) {
      SG::ReadCondHandle<LArBadChannelCont> bcContHdl{m_bcContKey,ctx};
      bcCont=(*bcContHdl);
    }


    for (unsigned hs=0;hs<hashMax;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      const Identifier id=cabling->cnvToIdentifier(chid);
      ATH_MSG_DEBUG ( "cell id: " << id << " " << cabling->isOnlineConnected(chid) );

      if(!cabling->isOnlineConnected(chid)){
	ATH_MSG_DEBUG ( "cell id: " << id << " not connected channel, skip  " );
	// Same (very high) thresholds for disconnected channels as masked channels
	ptQThrBlob[hs]=m_maskedtqThrsh;
	psamplesBlob[hs]=m_maskedsamplesThrsh;
	ptrigSumBlob[hs]=m_maskedtqThrsh;
	continue;
      }

      if(m_maskBadChannels && m_bcMask.cellShouldBeMasked(bcCont,chid)){ // Default gain is CaloGain::UNKNOWNGAIN
	ATH_MSG_DEBUG ( "cell id: " << id << " is masked; set thresholds to " << m_maskedtqThrsh << ", " << m_maskedsamplesThrsh );
	ptQThrBlob[hs]=m_maskedtqThrsh;
	psamplesBlob[hs]=m_maskedsamplesThrsh;
	ptrigSumBlob[hs]=m_maskedtqThrsh;
	continue;
      }

      if (m_workmode==FIXED) {
	//cont->set(chid,m_tqThrsh,m_samplesThrsh,m_tqThrsh); 
	ptQThrBlob[hs]=m_tqThrsh;
	psamplesBlob[hs]=m_samplesThrsh;
	ptrigSumBlob[hs]=m_tqThrsh;
      }
      else if (m_workmode==GROUP) {
	const std::vector<float>& thrs=m_thrPerCell.valuesForCell(id);
	if(thrs.size()!=2) std::cout << "len=" << thrs.size() << std::endl;
	ptQThrBlob[hs]=thrs[0];
	psamplesBlob[hs]=thrs[1];
	ptrigSumBlob[hs]=thrs[0];
      }
      else if (m_workmode==NOISE) {
	const CaloDetDescrElement *caloDDE = theCaloDDM->get_element(id);
	if(!caloDDE){
	  ATH_MSG_ERROR ( "Failed to return CaloDetDescrElement" );
	  return StatusCode::FAILURE;
	}
	/*
	  if(caloDDE->is_lar_em_barrel())
	  log << MSG::DEBUG << "EM barrel" << endmsg;
	  
	  if(caloDDE->is_lar_em_endcap())
	  log << MSG::DEBUG << "EM endcap" << endmsg;
	  
	  if(caloDDE->is_lar_hec())
	  log << MSG::DEBUG << "HEC" << endmsg;

	  if(caloDDE->is_lar_fcal())
	  log << MSG::DEBUG << "FCAL" << endmsg;
	*/
	CaloGain::CaloGain igain = CaloGain::INVALIDGAIN;

	if(caloDDE->is_lar_fcal())
	  igain = CaloGain::LARMEDIUMGAIN;
	else
	  igain = CaloGain::LARHIGHGAIN;

	ATH_MSG_DEBUG ( "hash, eta, phi: " << caloDDE->calo_hash() << ", " << caloDDE->eta() << ", " << caloDDE->phi() << "; noise: " << totalNoise->getNoise(id,igain) );

	float samplesThr = 0.;
	float QtThr = 0.;

	if(m_usePileupNoiseSamples)
	  samplesThr = totalNoise->getNoise(id,igain);
	else
	  samplesThr = elecNoise->getNoise(id,igain);

	if(m_usePileupNoiseQt)
	  QtThr = totalNoise->getNoise(id,igain);
	else
	  QtThr = elecNoise->getNoise(id,igain);

	//cont->set(chid,QtThr*m_sigmaNoiseQt,samplesThr*m_sigmaNoiseSamples,QtThr*m_sigmaNoiseQt);
	ptQThrBlob[hs]=QtThr*m_sigmaNoiseQt;
	psamplesBlob[hs]=samplesThr*m_sigmaNoiseSamples;
	ptrigSumBlob[hs]=QtThr*m_sigmaNoiseQt;

      }// end if NOISE
    }//end loop over cells
  }//end if FILL
  
  if (m_dump) {
  
    const AthenaAttributeList* attr=0;

    ATH_CHECK( detStore()->retrieve(attr,m_key) );

    const LArDSPThresholdsFlat* cont=new LArDSPThresholdsFlat(attr);


    std::ostream *out = &(std::cout); 
    std::ofstream outfile;
    if (m_outFileName.size()) {
      outfile.open(m_outFileName.c_str(),std::ios::out);
      if (outfile.is_open()) {
	ATH_MSG_INFO ( "Writing to file " << m_outFileName );
	out = &outfile;
      }
      else
	ATH_MSG_ERROR ( "Failed to open file " << m_outFileName );
    }

    std::vector<HWIdentifier>::const_iterator chanIt=m_onlineID->channel_begin();
    std::vector<HWIdentifier>::const_iterator chanIt_e=m_onlineID->channel_end ();
    for (;chanIt!=chanIt_e;++chanIt) {
      const HWIdentifier chid=*chanIt;    
      (*out) << std::fixed << chid.get_compact() << " " << cont->tQThr(chid) << " " << cont->samplesThr(chid) << " " << cont->trigSumThr(chid) 
	     << std::endl;

    } //end if loop over channels

    if (outfile.is_open())
      outfile.close();
    delete cont;
  }// end if m_dump

  return StatusCode::SUCCESS;
}
