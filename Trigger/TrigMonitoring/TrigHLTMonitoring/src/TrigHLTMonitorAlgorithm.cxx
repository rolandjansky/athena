/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTMonitorAlgorithm.h"

using namespace TrigCompositeUtils;

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

  auto tool = getGroup("TrigHLTMonitor");

  ////////////////////////////////////////
  // Declare the quantities which should be monitored
  // NB! The variables and histograms defined here must match the ones in the py file exactly!

  auto HLT_ElectronsRAW = Monitored::Scalar<int>("HLT_ElectronsRAW",0);
  auto HLT_AllChainsRAW = Monitored::Scalar<std::string>("HLT_AllChainsRAW");
  auto L1Events = Monitored::Scalar<std::string>("L1Events");


  ////////////////////////////////////
  // L1 items monitoring

  std::vector<std::string> L1items = m_trigDecTool->getChainGroup("L1_.*")->getListOfTriggers();
  unsigned int nL1Items = L1items.size();

  // Fill. First argument is the tool (GMT) name as defined in the py file, 
  // all others are the variables to be saved.
  // Alternative fill method. Get the group yourself, and pass it to the fill function.

  ATH_MSG_INFO("Filling L1Events histogram");
  for(unsigned int it=0; it<nL1Items; ++it) {
    if( L1items[it] != "" ) {
      
      ATH_MSG_DEBUG("L1Item " << it << " " << L1items << m_trigDecTool->isPassed(L1items[it]));
      if(m_trigDecTool->isPassed(L1items[it])) {
	
	/// Fill L1 histogram
	L1Events = L1items[it];
	ATH_MSG_DEBUG("L1Chain " << it << L1items[it]  << " is passed");
	fill(tool,L1Events);
      }
    }
  }


  ////////////////////////////////////
  // HLT chain monitoring

  //// Set the values of the monitored variables for the event		
  HLT_ElectronsRAW = GetEventInfo(ctx)->runNumber(); //DUMMY; to be updated.

  ATH_MSG_INFO( "Filling HLT_AllChains and RoI information" );
  std::vector< std::string > chainNames = m_trigDecTool->getChainGroup("HLT_.*")->getListOfTriggers();
  unsigned int nHLTChains = chainNames.size();

  for(unsigned int ith=0; ith<nHLTChains; ++ith) {
    if( chainNames[ith] != "" ) {

      ATH_MSG_DEBUG("HLTChain " << ith << " " << chainNames << m_trigDecTool->isPassed(chainNames[ith]));
      if(m_trigDecTool->isPassed(chainNames[ith])) {
	ATH_MSG_DEBUG( "    Chain " << chainNames[ith] << " IS passed");

	/// Fill plain chains histogram
	HLT_AllChainsRAW = chainNames[ith];
	ATH_MSG_DEBUG( "Fill HLT_AllChainsRAW" ); // with " << HLT_AllChainsRAW );
	fill(tool,HLT_AllChainsRAW);
	
	/// Fill RoIs histogram
	ATH_MSG_DEBUG("Fill RoI histograms for chain " << chainNames[ith] );
	std::vector<LinkInfo<TrigRoiDescriptorCollection>> fvec = m_trigDecTool->features<TrigRoiDescriptorCollection>(chainNames[ith], TrigDefs::Physics, "", TrigDefs::lastFeatureOfType, "initialRoI"); //initialRoiString()

	//Loop over RoIs
	for (const LinkInfo<TrigRoiDescriptorCollection>& li : fvec) {

	  if( li.isValid() ) {
	    auto phi = Monitored::Scalar("phi", 0.0);
	    auto eta = Monitored::Scalar("eta", 0.0);
	    auto HLT_AllChainsRoIs = Monitored::Group(tool, eta, phi); //filled when going out of scope

	    const TrigRoiDescriptor* roi = *(li.link).cptr();
    	    eta = roi->eta();
	    phi = roi->phi();
	    ATH_MSG_DEBUG( "RoI: eta = " << eta << ", phi = " << phi ); 
	  }

	  else {
	    ATH_MSG_WARNING( "TrigRoiDescriptorCollection for chain " << chainNames[ith] << " is not valid");
	  }

	}//end for (const LinkInfo<TrigRoiDescriptorCollection>& li : fvec)
      }// end if(m_trigDecTool->isPassed(chainNames[ith]))
    }// end if( chainNames[ith] != "" )
  }//end for(unsigned int ith=0; ith<nHLTChains; ++ith)


  //Placeholder   
  ATH_MSG_DEBUG( "Fill HLT_ElectronsRAW with " << HLT_ElectronsRAW );
  fill(tool,HLT_ElectronsRAW);
  

  //////////////////////////////////////
  // HLTResult and ConfigConsistency
  
  sc = fillResultAndConsistencyHistograms(ctx);
  
  
  //////////////////////////////////////
  // End
  ATH_MSG_INFO( "Finalizing the TrigHLTMonitorAlgorithm..." );
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
    ATH_MSG_INFO("No HLTResult monitored (because it has not been implemented yet)");
    //FIXME: When the HLTResult IS implemented this should be a WARNING // ebergeas Sept 2020
    //ATH_MSG_WARNING("No HLTResult found"); //Prints a warning message in the log file
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


