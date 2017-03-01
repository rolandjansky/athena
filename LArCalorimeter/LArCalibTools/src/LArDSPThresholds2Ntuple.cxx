/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArDSPThresholds2Ntuple.h"
#include "LArIdentifier/LArOnlineID.h"

#include "LArRawConditions/LArDSPThresholdsComplete.h"
#include "LArCOOLConditions/LArDSPThresholdsFlat.h"

#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

LArDSPThresholds2Ntuple::LArDSPThresholds2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator){

  declareProperty("NtupleTitle",m_ntTitle="Gain Thresholds");
  declareProperty("NtupleName",m_ntname="DSPTHR");
  declareProperty("DumpFlat",m_doFlat=false);
  declareProperty("FlatFolder",m_folder="");
  m_ntpath="/NTUPLES/FILE1/"+m_ntname;
  m_addFEBTemp=false;
}


StatusCode LArDSPThresholds2Ntuple::initialize() {

  StatusCode sc=LArCond2NtupleBase::initialize();
   if (sc!=StatusCode::SUCCESS) {
     msg(MSG::ERROR) << "Base init failed" << endmsg;
     return StatusCode::FAILURE;
   }

   sc=m_nt->addItem("tQThr",m_tQThr);
   if (sc!=StatusCode::SUCCESS) {
     msg(MSG::ERROR) << "addItem tQThr failed" << endmsg;
     return StatusCode::FAILURE;
   }
   
   sc=m_nt->addItem("samplesThr",m_samplesThr);
   if (sc!=StatusCode::SUCCESS) {
     msg(MSG::ERROR) << "addItem samplesThr failed" << endmsg;
     return StatusCode::FAILURE;
   }
   
   sc=m_nt->addItem("trigThr",m_trigThr);
   if (sc!=StatusCode::SUCCESS) {
     msg(MSG::ERROR) << "addItem trigThr failed" << endmsg;
     return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;
}

LArDSPThresholds2Ntuple::~LArDSPThresholds2Ntuple() 
{}

StatusCode LArDSPThresholds2Ntuple::stop() {
  StatusCode sc;
   
   const LArDSPThresholdsComplete *dc = 0;
   const LArDSPThresholdsFlat *df = 0;
   const AthenaAttributeList* attrList=0;
   if(m_doFlat) {
      sc=detStore()->retrieve(attrList,m_folder);
      if (sc.isFailure()) {
         msg(MSG::ERROR) << "Failed to retrieve AthenaAttributeList with key " << m_folder << endmsg;
         return sc;
      }
      
      const coral::Blob& blob = (attrList->coralList())["tQThr"].data<coral::Blob>();
      if (blob.size()<3) {
        msg(MSG::INFO) << "Found empty blob, nothing to do"<<endmsg;
        return StatusCode::SUCCESS;
      }
      
      df=new LArDSPThresholdsFlat(attrList);

   } else {
      sc=m_detStore->retrieve(dc);
      if(sc!=StatusCode::SUCCESS) {
           msg(MSG::ERROR) <<"Could not retrieve LArDSPThresholdsComplete...."<<endmsg;
           return StatusCode::FAILURE;
      }
   }
   
   std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();

   for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     if(m_doFlat) {
        m_tQThr=df->tQThr(hwid);
        m_samplesThr=df->samplesThr(hwid);
        m_trigThr=df->trigSumThr(hwid);
     } else {
         m_tQThr=dc->tQThr(hwid);
         m_samplesThr=dc->samplesThr(hwid);
         m_trigThr=dc->trigSumThr(hwid);
     }
     
     fillFromIdentifier(hwid);
     //msg(MSG::INFO)<<"hwid: "<<hwid.getString()<<" "<<tQThr<<" : "<<samplesThr<<" : "<<trigThr<<endmsg;
     
     sc=ntupleSvc()->writeRecord(m_nt);      
     if (sc!=StatusCode::SUCCESS) {
       msg(MSG::ERROR) << "writeRecord failed" << endmsg;
       return StatusCode::FAILURE;
     }
   }
   
 
   msg(MSG::INFO) << "LArDSPThresholds2Ntuple has finished." << endmsg;
   return StatusCode::SUCCESS;
   
}// end finalize-method.
   
