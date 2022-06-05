/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class FixLArElecSCCalib
   @based on FixLArElecCalib from Hong Ma <hma@bnl.gov>
   @brief fix electronic calibration constants for SC

   This should be used with FixLArElecSCCalib_jobOptions.py to write a new
   POOL file.

   Implemented fixes:
   
   1. Flag = 1
	scale values for Pedestals and Ramps by factor
   2. Flag = 2
	create calib. line mapping for SC
 
 
**/ 


#include "FixLArElecSCCalib.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "CaloIdentifier/CaloGain.h"

#include "LArCOOLConditions/LArPedestalSC.h"
#include "LArCOOLConditions/LArRampSC.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include <fstream>

FixLArElecSCCalib::FixLArElecSCCalib(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator),
  m_fixFlag(0),
  m_em_idhelper(0),
  m_hec_idhelper(0),
  m_fcal_idhelper(0),
  m_online_idhelper(0),
  m_sem_idhelper(0),
  m_shec_idhelper(0),
  m_sfcal_idhelper(0),
  m_sonline_idhelper(0),
  m_scell_idhelper(0)
{ 

    declareProperty("FixFlag",      m_fixFlag);
    declareProperty("FixFactor",    m_fixFactor);

}

FixLArElecSCCalib::~FixLArElecSCCalib()
{ }

StatusCode FixLArElecSCCalib::initialize() {
  ATH_MSG_INFO ( " in initialize " );
  
  ATH_CHECK( detStore()->retrieve(m_em_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_hec_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_fcal_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_sem_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_shec_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_sfcal_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_online_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_sonline_idhelper) );
  ATH_CHECK( detStore()->retrieve(m_scell_idhelper) );

  ATH_CHECK( m_cablingKeySC.initialize() );
  ATH_CHECK( m_CLKeySC.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode FixLArElecSCCalib::execute() {

  ATH_MSG_INFO ( " in execute  " );
  return StatusCode::SUCCESS;
}

StatusCode FixLArElecSCCalib::stop() {

   ATH_MSG_INFO ( " in stop " );

    switch(m_fixFlag) {
       case 1: return fix1(); 
       case 2: {
                  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKeySC};
                  const LArOnOffIdMapping* cabling{*cablingHdl};
                  if(!cabling) {
                      ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKeySC.key() );
                      return StatusCode::FAILURE;
                  }
                  SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_CLKeySC};
                  const LArCalibLineMapping *clCont {*clHdl};
                  if(!clCont) {
                     ATH_MSG_ERROR( "Do not have calib line mapping !!!" );
                     return StatusCode::FAILURE;
                  }
                  return fix2(cabling, clCont);
               }
       default: return StatusCode::SUCCESS;
    }        
}

StatusCode FixLArElecSCCalib::fix1() {

   ATH_MSG_INFO ( " in fix1() " );

   // Fix1 is for scaling the Pedestals and Rams 
   int n=0;

   const LArPedestalSC *ped = nullptr;
   CHECK(detStore()->retrieve(ped, "Pedestal_orig"));

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("Pedestal", "blob");
  spec->extend("PedestalRMS", "blob");
  spec->extend<unsigned>("version");

  CondAttrListCollection* collPed=new CondAttrListCollection(true);

  unsigned gain=0;
  unsigned hashMax=m_sonline_idhelper->channelHashMax();
    coral::AttributeList* attrList = new coral::AttributeList(*spec);               
    (*attrList)["version"].setValue(0U);                               
    coral::Blob& blobPed=(*attrList)["Pedestal"].data<coral::Blob>();
    coral::Blob& blobRMS=(*attrList)["PedestalRMS"].data<coral::Blob>();
    blobPed.resize(hashMax*sizeof(float));
    blobRMS.resize(hashMax*sizeof(float));
    float* pblobPed=static_cast<float*>(blobPed.startingAddress());
    float* pblobRMS=static_cast<float*>(blobRMS.startingAddress());

    for (unsigned hs=0;hs<hashMax;++hs) {
      const HWIdentifier chid=m_sonline_idhelper->channel_Id(hs);
      float p=ped->pedestal(chid,gain);
      float rms=ped->pedestalRMS(chid,gain);

      pblobPed[hs]=p*m_fixFactor;
      pblobRMS[hs]=rms;
      ++n; 
    }//end loop over hash ids
    collPed->add(gain,*attrList);
   
  StatusCode sc=detStore()->record(collPed,"/LAR/ElecCalibMCSC/Pedestal");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection for pedestal "  );
    return sc;
  }

   ATH_MSG_DEBUG("  Number of entries changes in pedestals =  " <<n );

   // Now ramp
   n=0;
   const LArRampSC *ramp = nullptr;
   CHECK(detStore()->retrieve(ramp, "Ramp_orig"));

  spec = new coral::AttributeListSpecification();
  spec->extend("RampVec", "blob");
  spec->extend<unsigned>("nPoints");
  spec->extend<unsigned>("version");
  CondAttrListCollection* coll=new CondAttrListCollection(true);


  std::vector<float> defaultRamp={0.0,1.0};

  gain=0;

    unsigned nPoints=0;
    unsigned nDefault=0;
    for (unsigned hs=0;hs<hashMax && nPoints==0;++hs) {
      const HWIdentifier chid=m_sonline_idhelper->channel_Id(hs);
      const ILArRamp::RampRef_t rampref= ramp->ADC2DAC(chid,gain);
      nPoints=rampref.size();
    }
    if (nPoints==0) {
      ATH_MSG_ERROR( "All input Ramps for gain " << gain << " have 0 points!" );
      return StatusCode::FAILURE;
    }

    defaultRamp.resize(nPoints,0.0); //fill remaining points if needed
    ATH_MSG_DEBUG( "Gain " << gain << ": Found a ramp polynom of degree " << nPoints << " in input data" );
    attrList = new coral::AttributeList(*spec);
    (*attrList)["version"].setValue(0U);
    coral::Blob& blobRamp=(*attrList)["RampVec"].data<coral::Blob>();
    (*attrList)["nPoints"].setValue(nPoints);
    blobRamp.resize(hashMax*sizeof(float)*nPoints);
    float* pblobRamp=static_cast<float*>(blobRamp.startingAddress());

    for (unsigned hs=0;hs<hashMax;++hs) {
      const HWIdentifier chid=m_sonline_idhelper->channel_Id(hs);
      std::vector<float> rampVec(ramp->ADC2DAC(chid,gain).asVector());
      
      if (rampVec.size()>=nPoints) {
	for (size_t i=0;i<nPoints;++i) {
	  pblobRamp[nPoints*hs+i]=rampVec[i]*m_fixFactor;
	}
      }
      else {
	std::stringstream message;
	message <<"Polynom degree doesn't match. Expect " << nPoints << ", got " << rampVec.size() << "."; 
	for (size_t i=0;i<nPoints;++i) {
	  pblobRamp[nPoints*hs+i]=defaultRamp[i];
	}
	++nDefault;
      }
      ++n;
    }//end loop over hash ids
    coll->add(gain,*attrList);
  
  sc=detStore()->record(coll,"/LAR/ElecCalibMCSC/Ramp");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection for ramp " );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( "Converted Ramps to inline storage. Total number of channels " << n );
  ATH_MSG_INFO( "Number of channels filled with default ramp {0,1} " << nDefault << " (including disconnected)" );

   return StatusCode::SUCCESS;
}

StatusCode FixLArElecSCCalib::fix2(const LArOnOffIdMapping *cabling, const LArCalibLineMapping *cl) {

   ATH_MSG_INFO ( " in fix2() " );

   const uint32_t onlHashMax=m_sonline_idhelper->channelHashMax(); // all SC cells

   coral::AttributeListSpecification* spec_calib = new coral::AttributeListSpecification();
   spec_calib->extend("OnlineHashToCalibIds", "blob");
   spec_calib->extend<unsigned>("version");
   AthenaAttributeList* al_calib = new AthenaAttributeList(*spec_calib);
   coral::Blob& blobCalib=(*al_calib)["OnlineHashToCalibIds"].data<coral::Blob>();
   (*al_calib)["version"].setValue(0U);
   blobCalib.resize(onlHashMax*sizeof(uint32_t)*5); //Bigger than necessary 
   ATH_MSG_DEBUG("blobCalib size: "<<onlHashMax<<"*5");

   spec_calib->release();
   spec_calib = nullptr;

   uint32_t* pBlobCalib=static_cast<uint32_t*>(blobCalib.startingAddress());

  size_t calibIndex=0;

  std::vector<unsigned> calibHist(17,0);
  unsigned calibHistMax=0;

  std::ofstream outfile("SCIdentifiers.txt");

  outfile << "hash id bec pn FT SL chan id calo pn sampl reg eta phi calib" << std::endl;

  for (uint32_t onlHash=0;onlHash<onlHashMax;++onlHash) {
    const HWIdentifier hwid=m_sonline_idhelper->channel_Id(onlHash);

    const std::vector<HWIdentifier>& calibIDs_tmp=cl->calibSlotLine(hwid);
    std::vector<HWIdentifier> calibIDs;
    for (unsigned i=0; i<calibIDs_tmp.size(); ++i) {
       if(std::find(calibIDs.begin(), calibIDs.end(), calibIDs_tmp[i]) == calibIDs.end()) calibIDs.push_back(calibIDs_tmp[i]);
    }
    // deduplicate
    Identifier *id= new Identifier();
    if (cabling->isOnlineConnected(hwid)) {
       *id=cabling->cnvToIdentifier(hwid);
       print(hwid,dynamic_cast<const LArOnlineID_Base*>(m_sonline_idhelper), dynamic_cast<const CaloCell_Base_ID*>(m_scell_idhelper),
          id,&calibIDs,outfile);
    } else {
       print(hwid,dynamic_cast<const LArOnlineID_Base*>(m_sonline_idhelper), dynamic_cast<const CaloCell_Base_ID*>(m_scell_idhelper),
          nullptr,&calibIDs,outfile);
    }
    const size_t nCalibLines=calibIDs.size();
    if (nCalibLines > calibHistMax ) calibHistMax=nCalibLines;
    if(calibHistMax > 17) {
       ATH_MSG_ERROR( "Too much calib lines, adjust please !!!" << calibHistMax);
       return StatusCode::FAILURE;
    }
    (calibHist[nCalibLines])++;
    pBlobCalib[calibIndex++]=nCalibLines;
    for(uint32_t iCalib=0;iCalib<nCalibLines;++iCalib)
      pBlobCalib[calibIndex++]=calibIDs[iCalib].get_identifier32().get_compact();
  }

  blobCalib.resize(calibIndex*sizeof(uint32_t)); //Size down to actual size


  outfile.close();

  ATH_MSG_INFO( "calibHistMax: " << calibHistMax);
  ATH_MSG_INFO( "BlobSize CalibId:" << calibIndex);
  msg(MSG::INFO) << "nCalib[i] ";
  for (unsigned j=0;j<17;++j)
    msg() << calibHist[j] << "/";
  msg() << endmsg;

  ATH_CHECK(detStore()->record(al_calib,"/LAR/IdentifierOfl/CalibIdMap_SC"));

  return StatusCode::SUCCESS;
}

void FixLArElecSCCalib::print (const HWIdentifier& hwid, const LArOnlineID_Base* onlineID, const CaloCell_Base_ID* caloCellID,
                               const Identifier *id, std::vector<HWIdentifier>* calibIDs, std::ostream& out) {
  const IdentifierHash hwid_hash=onlineID->channel_Hash(hwid);
  out << hwid_hash << " " << std::hex << "0x" << hwid.get_identifier32().get_compact() << std::dec << " "
      << onlineID->barrel_ec(hwid) << " "
      << onlineID->pos_neg(hwid) << " "
      << onlineID->feedthrough(hwid) << " "
      << onlineID->slot(hwid) << " "
      << onlineID->channel(hwid) << " : ";
  if (id) {
    out   << std::hex << "0x" << id->get_identifier32().get_compact() << std::dec << " "
          << caloCellID->sub_calo(*id) << " "
          << caloCellID->pos_neg(*id) << " "
          << caloCellID->sampling(*id) << " "
          << caloCellID->region(*id) << " "
          << caloCellID->eta(*id) << " "
          << caloCellID->phi(*id) << " ";
  }
  else
    out << " disconnected ";

  if(calibIDs){
    for (size_t i=0;i<calibIDs->size();++i) {
      out << std::hex << "0x" << calibIDs->at(i).get_identifier32().get_compact() << " ";
    }
  } else 
    out << " no calib ";

  out << std::dec << std::endl;
}
