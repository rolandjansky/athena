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
     ATH_MSG_ERROR( "Base init failed" );
     return StatusCode::FAILURE;
   }

   sc=m_nt->addItem("peakSample",m_peakSample);
   if (sc!=StatusCode::SUCCESS) {
     ATH_MSG_ERROR( "addItem peakSample failed" );
     return StatusCode::FAILURE;
   }
   
   sc=m_nt->addItem("useHGIntercept",m_useHgIntercept);
   if (sc!=StatusCode::SUCCESS) {
     ATH_MSG_ERROR( "addItem useHGIntercept failed" );
     return StatusCode::FAILURE;
   }
   
   sc=m_nt->addItem("useMGIntercept",m_useMgIntercept);
   if (sc!=StatusCode::SUCCESS) {
     ATH_MSG_ERROR( "addItem useMGIntercept failed" );
     return StatusCode::FAILURE;
   }
   
   sc=m_nt->addItem("useLGIntercept",m_useLgIntercept);
   if (sc!=StatusCode::SUCCESS) {
     ATH_MSG_ERROR( "addItem useLGIntercept failed" );
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
     ATH_MSG_ERROR( "Failed to retrieve AthenaAttributeList with key " << m_folder );
     return sc;
   }
      
   LArDSPConfig larDSPConfig(attrList);

      
   std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();

   for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     m_peakSample=larDSPConfig.peakSample(hwid);
     //ATH_MSG_INFO("hwid: "<<hwid.getString()<<" "<<m_peakSample);
     m_useMgIntercept=larDSPConfig.useMGRampIntercept(hwid);
     m_useHgIntercept=larDSPConfig.useHGRampIntercept(hwid);
     m_useLgIntercept=larDSPConfig.useLGRampIntercept(hwid);
     //ATH_MSG_INFO("hwid: "<<hwid.getString()<<" "<<m_useHgIntercept<<" "<<m_useMgIntercept<<" "<<m_useLgIntercept);
     fillFromIdentifier(hwid);
     //ATH_MSG_INFO("hwid: "<<hwid.getString()<<" "<<tQThr<<" : "<<samplesThr<<" : "<<trigThr);
     
     sc=ntupleSvc()->writeRecord(m_nt);      
     if (sc!=StatusCode::SUCCESS) {
       ATH_MSG_ERROR( "writeRecord failed" );
       return StatusCode::FAILURE;
     }
   }
   
   ATH_MSG_INFO( "LArDSPConfig2Ntuple has finished." );
   return StatusCode::SUCCESS;
   
}// end finalize-method.
   
