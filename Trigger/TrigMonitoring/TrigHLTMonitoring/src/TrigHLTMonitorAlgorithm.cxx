/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTMonitorAlgorithm.h"

TrigHLTMonitorAlgorithm::TrigHLTMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
  , m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool") 
{
}


TrigHLTMonitorAlgorithm::~TrigHLTMonitorAlgorithm() {}


StatusCode TrigHLTMonitorAlgorithm::initialize() {

  ATH_CHECK( m_trigDecTool.retrieve() );
  ATH_CHECK( m_hltResultReadKey.initialize() );
  ATH_CHECK( m_trigConfigSvc.retrieve() );

  return AthMonitorAlgorithm::initialize();
}


StatusCode TrigHLTMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;
  StatusCode sc = StatusCode::FAILURE;

  ////////////////////////////////////
  // Chain monitoring

  // Declare the quantities which should be monitored
  //NB! The variables and histograms defined here must match the ones in the py file exactly!
  auto placeholder_ele = Monitored::Scalar<int>("placeholder_ele",0);
  auto placeholder_hlt = Monitored::Scalar<int>("placeholder_hlt",0);
  
  //// Set the values of the monitored variables for the event		
  placeholder_ele = GetEventInfo(ctx)->runNumber(); //DUMMY; to be updated.
  placeholder_hlt = GetEventInfo(ctx)->runNumber(); //DUMMY; to be updated.
  
  // Fill. First argument is the tool (GMT) name as defined in the py file, 
  // all others are the variables to be saved.
  // Alternative fill method. Get the group yourself, and pass it to the fill function.
  auto tool = getGroup("TrigHLTMonitor");
  
  fill(tool,placeholder_hlt);
  fill(tool,placeholder_ele);
  
  
  //////////////////////////////////////
  // HLTResult and ConfigConsistency
  
  sc = fillResultAndConsistencyHistograms(ctx);
  
  
  //////////////////////////////////////
  // End
  return StatusCode::SUCCESS;
}


StatusCode TrigHLTMonitorAlgorithm::fillResultAndConsistencyHistograms( const EventContext& ctx ) const {
  using namespace Monitored;
  StatusCode sc_trigDec = StatusCode::FAILURE; 
  StatusCode sc_hltResult = StatusCode::FAILURE; 

  // Declare the quantities which should be monitored
  //NB! The variables and histograms defined here must match the ones in the py file exactly!
  auto HLTResultHLT = Monitored::Scalar<int>("HLTResultHLT",0);
  auto ConfigConsistency_HLT = Monitored::Scalar<int>("ConfigConsistency_HLT",0);

  SG::ReadHandle<HLT::HLTResultMT> resultHandle = SG::makeHandle( m_hltResultReadKey, ctx );


  //////////////////////////////////////
  // HLTResult and ConfigConsistency

  //Use the m_trigConfigSvc rather than the m_configTool. You should remove the m_configTool.
  //But use the TrigConf::xAODConfigSvc rather than the old TrigConf::TrigConfigSvc/TrigConfigSvc
    
  uint32_t bskeys_1 = 9999; uint32_t bskeys_2 = 9999;
  HLTResultHLT = 1; //DUMMY; to be updated.
  sc_hltResult = StatusCode::FAILURE; //no HLTResult yet

  
  // Fill. First argument is the tool (GMT) name as defined in the py file, 
  // all others are the variables to be saved.
  // Alternative fill method. Get the group yourself, and pass it to the fill function.
  auto tool = getGroup("TrigHLTMonitor");

    
  if(sc_hltResult == StatusCode::SUCCESS) {
    //THIS IS NOT IMPLEMENTED YET IN Run3
    //bskeys_1 = HLTResult->getConfigSuperMasterKey();
    //bskeys_2 = HLTResult->getConfigPrescalesKey();
    ATH_MSG_INFO("sc_hltResult should not be SUCCESS");
  }
  else {
    //For now, this will always happen    
    ATH_MSG_WARNING("No HLTResult found"); //Prints a warning message in the log file
    ConfigConsistency_HLT=7;
    fill(tool,ConfigConsistency_HLT); //Fills the warning bin in the ConfigConsistency histogram
    bskeys_1 = 0;
    bskeys_2 = 0;
    ATH_MSG_DEBUG("bskeys_1 = " << bskeys_1 << ", bskeys_2 = " << bskeys_2);

  }

  
  uint32_t bskeys[] = {bskeys_1, bskeys_2};
  uint32_t dbkeys[2];
  dbkeys[0]=m_trigConfigSvc->masterKey();
  dbkeys[1]=m_trigConfigSvc->hltPrescaleKey();
  for(int i = 0; i < 2; ++i) {
    ATH_MSG_DEBUG(" ===> Filling ConfigConsistency_HLT");
    ATH_MSG_DEBUG("i = " << i << ", dbkeys[" << i << "] = " << dbkeys[i] << ", bskeys[" << i << "] = " << bskeys[i]); 
    if(dbkeys[i]==0) {
      ConfigConsistency_HLT=3*i+1;
      fill(tool,ConfigConsistency_HLT);
      ATH_MSG_DEBUG("dbkeys[" << i << "] = 0, ConfigConsistency_HLT=" << ConfigConsistency_HLT);
    }
    if(bskeys[i]==0) {
      ConfigConsistency_HLT=3*i+2;
      fill(tool,ConfigConsistency_HLT);
      ATH_MSG_DEBUG("bskeys[" << i << "] = 0, ConfigConsistency_HLT=" << ConfigConsistency_HLT);
    }
    if(dbkeys[i]!=bskeys[i]) {
      ConfigConsistency_HLT=3*i+3;
      fill(tool,ConfigConsistency_HLT);
      ATH_MSG_DEBUG("dbkeys[" << i << "]!=bskeys[" << i << "], ConfigConsistency_HLT=" << ConfigConsistency_HLT);
    } 
  }
    

  fill(tool,HLTResultHLT);

  return StatusCode::SUCCESS;
}


