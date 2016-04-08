/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArDSPConfig2Ntuple.h"
#include "LArIdentifier/LArOnlineID.h"

#include "LArCOOLConditions/LArDSPConfig.h"

#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

LArDSPConfig2Ntuple::LArDSPConfig2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator){

  declareProperty("NtupleTitle",m_ntTitle="DSP Config");
  declareProperty("NtupleName",m_ntname="DSPCONFIG");
  declareProperty("Folder",m_folder);
  m_ntpath="/NTUPLES/FILE1/"+m_ntname;
  m_addFEBTemp=false;
}


StatusCode LArDSPConfig2Ntuple::initialize() {

  StatusCode sc=LArCond2NtupleBase::initialize();
   if (sc!=StatusCode::SUCCESS) {
     msg(MSG::ERROR) << "Base init failed" << endreq;
     return StatusCode::FAILURE;
   }

   sc=m_nt->addItem("peakSample",m_peakSample);
   if (sc!=StatusCode::SUCCESS) {
     msg(MSG::ERROR) << "addItem peakSample failed" << endreq;
     return StatusCode::FAILURE;
   }
   
   sc=m_nt->addItem("useMGIntercept",m_useMgIntercept);
   if (sc!=StatusCode::SUCCESS) {
     msg(MSG::ERROR) << "addItem useMGIntercept failed" << endreq;
     return StatusCode::FAILURE;
   }
   
   return StatusCode::SUCCESS;
}

LArDSPConfig2Ntuple::~LArDSPConfig2Ntuple() 
{}

StatusCode LArDSPConfig2Ntuple::stop() {
  StatusCode sc;

   
  //const LArDSPConfig *dc = 0;
   const AthenaAttributeList* attrList=0;
   sc=detStore()->retrieve(attrList,m_folder);
   if (sc.isFailure()) {
     msg(MSG::ERROR) << "Failed to retrieve AthenaAttributeList with key " << m_folder << endreq;
     return sc;
   }
      
   LArDSPConfig larDSPConfig(attrList);

      
   std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();

   for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     m_peakSample=larDSPConfig.peakSample(hwid);
     m_useMgIntercept=larDSPConfig.useMGRampIntercept(hwid);
     fillFromIdentifier(hwid);
     //msg(MSG::INFO)<<"hwid: "<<hwid.getString()<<" "<<tQThr<<" : "<<samplesThr<<" : "<<trigThr<<endreq;
     
     sc=ntupleSvc()->writeRecord(m_nt);      
     if (sc!=StatusCode::SUCCESS) {
       msg(MSG::ERROR) << "writeRecord failed" << endreq;
       return StatusCode::FAILURE;
     }
   }
   
   msg(MSG::INFO) << "LArDSPConfig2Ntuple has finished." << endreq;
   return StatusCode::SUCCESS;
   
}// end finalize-method.
   
