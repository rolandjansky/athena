/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArGainThresholds2Ntuple.h"
#include "LArIdentifier/LArOnlineID.h"

LArGainThresholds2Ntuple::LArGainThresholds2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator) {

  m_ntTitle="Gain Thresholds";
  m_ntpath="/NTUPLES/FILE1/GAINTH";
}


StatusCode LArGainThresholds2Ntuple::initialize() {

  ATH_CHECK(m_configKey.initialize());

  return LArCond2NtupleBase::initialize();
}

LArGainThresholds2Ntuple::~LArGainThresholds2Ntuple() 
{}

StatusCode LArGainThresholds2Ntuple::stop() {

   ATH_MSG_DEBUG(" trying stop");

   NTuple::Item<long> lower;
   NTuple::Item<long> upper;
 
   SG::ReadCondHandle<LArFebConfig> configHdl{m_configKey};
   const LArFebConfig* febConfig = *configHdl;
   if (febConfig==nullptr) {
     ATH_MSG_ERROR( "Unable to retrieve LArFebConfig with key " << m_configKey.key());
     return StatusCode::FAILURE;
   }

   ATH_CHECK(m_nt->addItem("lower",lower,-1000,5000));
   ATH_CHECK(m_nt->addItem("upper",upper,-1000.,5000.));
   
   std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
   for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     lower=febConfig->lowerGainThreshold(hwid);
     upper=febConfig->upperGainThreshold(hwid);

     fillFromIdentifier(hwid);
     
     ATH_CHECK(ntupleSvc()->writeRecord(m_nt));      
   }
 
   ATH_MSG_INFO("LArGainThresholds2Ntuple has finished.");
   return StatusCode::SUCCESS;
   
}
