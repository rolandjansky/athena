/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MonitorConditionsTestAlg.cxx
 *
 * @brief Implementation file for the SCT_MonitorConditionsTestAlg class 
 * in package SCT_ConditionsAlgorithms
 *
 * @author Kazu Hanagaki
 **/

#include "SCT_MonitorConditionsTestAlg.h"

//Athena includes
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "StoreGate/ReadHandle.h"

SCT_MonitorConditionsTestAlg::SCT_MonitorConditionsTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm(name, pSvcLocator),
  m_sctId{nullptr}
{
}

// -------------------------------------------------------------------------

StatusCode SCT_MonitorConditionsTestAlg::initialize()
{

  ATH_CHECK(detStore()->retrieve(m_sctId, "SCT_ID"));
  ATH_MSG_DEBUG("Found SCT_ID Tool");

  ATH_CHECK(m_pMonitorConditionsTool.retrieve());
  ATH_MSG_DEBUG("Found SCT_MoniotorConditinosSvc");

  // Read Handle
  ATH_CHECK(m_evtKey.initialize());

  return StatusCode::SUCCESS;

}

StatusCode SCT_MonitorConditionsTestAlg::execute(const EventContext& ctx) const
{
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  //  A) Writing conditions data to the database
  //     * This is no longer done in this package so the test has been removed * 
  //  B) Reading back in conditions data to analyse it
  //
  //    1) Aside from specifying the correct jobOptions, this is
  //       simply done by doing a standard StoreGate retrieve from
  //       the DetectorStore.

  ATH_MSG_DEBUG(" in execute()");

  SG::ReadHandle<xAOD::EventInfo> evt{m_evtKey, ctx};
  if (not evt.isValid()) {
    ATH_MSG_FATAL("could not get event info ");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Event: [" << evt->runNumber()
                  << "," << evt->eventNumber()
                  << ":" << evt->timeStamp()
                  << "]");
  }
 
  std::string defectlist; 
  std::string EfficiencyTable;
  Identifier stripid1;
  Identifier waferid1;
  Identifier moduleid1;

  // For testing of reading back from database
  ATH_MSG_DEBUG("Testing isGood() function");

  SCT_ID::const_id_iterator waferItr{m_sctId->wafer_begin()};
  SCT_ID::const_id_iterator waferItrE{m_sctId->wafer_end()};
  int n_bad{0};
  for (; waferItr != waferItrE; ++waferItr ) {
    Identifier waferId{*waferItr};
    for (int i{0}; i<768; i++){
      Identifier stripId{m_sctId->strip_id(waferId, i)};
      if (not (m_pMonitorConditionsTool->isGood(stripId, InDetConditions::SCT_STRIP)))
	n_bad++;
    }
  }
  ATH_MSG_DEBUG("(MonitorTest): #bad strips(Total) = " << n_bad);

  // check if strip is noisy
  stripid1 = m_sctId->strip_id(0, 3, 41, -4, 1, 703);
  waferid1 = m_sctId->wafer_id(stripid1);
  moduleid1 = m_sctId->module_id(waferid1);
  ATH_MSG_DEBUG("(MonitorTest): stripid  = " << stripid1);
  ATH_MSG_DEBUG("(MonitorTest): moduleid = " << moduleid1);
  //    SCT_ComponentIdentifier compid = SCT_ComponentIdentifier(stripid1,"STRIP");
  //    SCT_Conditions::SCT_ComponentIdentifier compid(stripid1,"STRIP");
  bool isthisGood{m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_STRIP)};
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): strip(0,3,41,-4,1,703) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): strip(0,3,41,-4,1,703) is noisy ");
  }
  
  stripid1 = m_sctId->strip_id(0, 3, 41, -4, 0, 703);
  waferid1 = m_sctId->wafer_id(stripid1);
  moduleid1 = m_sctId->module_id(waferid1);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_STRIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): strip(0,3,41,-4,0,703) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): strip(0,3,41,-4,0,703) is noisy ");
  }
  
  // Added 04/06/09
  stripid1 = m_sctId->strip_id(0, 2, 39, -1, 0, 397);
  waferid1 = m_sctId->wafer_id(stripid1);
  moduleid1 = m_sctId->module_id(waferid1);
  ATH_MSG_DEBUG("(MonitorTest): stripid  = " << stripid1);
  ATH_MSG_DEBUG("(MonitorTest): moduleid = " << moduleid1);
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_STRIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): strip(0,2,39,-1,0,397) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): strip(0,2,39,-1,0,397) is noisy ");
  }
  
  stripid1 = m_sctId->strip_id(0, 2, 39, -1, 0, 396);
  waferid1 = m_sctId->wafer_id(stripid1);
  moduleid1 = m_sctId->module_id(waferid1);
  ATH_MSG_DEBUG("(MonitorTest): stripid  = " << stripid1);
  ATH_MSG_DEBUG("(MonitorTest): moduleid = " << moduleid1);
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_STRIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): strip(0,2,39,-1,0,396) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): strip(0,2,39,-1,0,396) is noisy ");
  }
  
  stripid1 = m_sctId->strip_id(0, 2, 39, -1, 0, 398);
  waferid1 = m_sctId->wafer_id(stripid1);
  moduleid1 = m_sctId->module_id(waferid1);
  ATH_MSG_DEBUG("(MonitorTest): stripid  = " << stripid1);
  ATH_MSG_DEBUG("(MonitorTest): moduleid = " << moduleid1);
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_STRIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): strip(0,2,39,-1,0,398) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): strip(0,2,39,-1,0,398) is noisy ");
  }
  
  stripid1 = m_sctId->strip_id(0, 3, 13, -3, 0, 567);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_STRIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): strip(0,3,13,-3,0,567) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): strip(0,3,13,-3,0,567) is noisy ");
  }
  
  stripid1 = m_sctId->strip_id(0, 3, 13, -3, 0, 566);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_STRIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): strip(0,3,13,-3,0,566) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): strip(0,3,13,-3,0,566) is noisy ");
  }
  
  stripid1 = m_sctId->strip_id(0, 3, 13, -3, 1, 567);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_STRIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): strip(0,3,13,-3,1,567) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): strip(0,3,13,-3,1,567) is noisy ");
  }

  stripid1 = m_sctId->strip_id(0, 0, 7, 2, 0, 700);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_STRIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): strip(0,0,7,2,0,700) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): strip(0,0,7,2,0,700) is noisy ");
  }
  
  stripid1 = m_sctId->strip_id(0, 0, 7, 2, 1, 700);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_STRIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): strip(0,0,7,2,1,700) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): strip(0,0,7,2,1,700) is noisy ");
  }

  // check if chip is noisy
  stripid1 = m_sctId->strip_id(0, 0, 8, -4, 0, 100);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"CHIP");
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_CHIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): chip(0,0,8,-4,0,100) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): chip(0,0,8,-4,0,100) is noisy ");
  }
  
  stripid1 = m_sctId->strip_id(0, 3, 13, -3, 0, 567);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"CHIP");
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_CHIP);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): chip(0,3,13,-3,0,567) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): chip(0,3,13,-3,0,567) is noisy ");
  }
  
  // check if wafer is noisy
  stripid1 = m_sctId->strip_id(0, 0, 8, -4, 0, 100);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"WAFER");
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_SIDE);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): wafer(0,0,8,-4,0,100) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): wafer(0,0,8,-4,0,100) is noisy ");
  }
  
  // check if module is noisy
  stripid1 = m_sctId->strip_id(0, 0, 8, -4, 0, 100);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"MODULE");
  isthisGood = m_pMonitorConditionsTool->isGood(stripid1, InDetConditions::SCT_MODULE);
  if (isthisGood) {
    ATH_MSG_INFO("isGood(): module(0,0,8,-4,0,100) is not noisy ");
  } else {
    ATH_MSG_INFO("isGood(): module(0,0,8,-4,0,100) is noisy ");
  }

  ATH_MSG_DEBUG("(MonitorTest): execute finished successfully.");
  return StatusCode::SUCCESS;
}

StatusCode SCT_MonitorConditionsTestAlg::finalize()
{
  ATH_MSG_DEBUG("SCT_MonitorConditionsTestAlg::finalize");

  return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------
