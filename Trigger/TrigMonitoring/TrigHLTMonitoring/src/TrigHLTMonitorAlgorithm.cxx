/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  ATH_CHECK( m_eventKey.initialize() );
  ATH_CHECK( m_onlineKey.initialize() );
  ATH_CHECK( m_trigConfigSvc.retrieve() );

  return AthMonitorAlgorithm::initialize();
}


StatusCode TrigHLTMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;
  StatusCode sc = StatusCode::FAILURE;

  //Fetch the general tool
  auto tool = getGroup("TrigHLTMonitor");


  ////////////////////////////////////////
  // Declare the quantities which should be monitored
  // NB! The variables and histograms defined here must match the ones in the py file exactly!

  auto L1Events = Monitored::Scalar<std::string>("L1Events");


  ////////////////////////////////////
  // L1 items monitoring

  std::vector<std::string> L1items = m_trigDecTool->getChainGroup("L1_.*")->getListOfTriggers();
  unsigned int nL1Items = L1items.size();

  // Fill. First argument is the tool (GMT) name as defined in the py file, 
  // all others are the variables to be saved.
  // Alternative fill method. Get the group yourself, and pass it to the fill function.

  ATH_MSG_DEBUG("Filling L1Events histogram");
  for(unsigned int it=0; it<nL1Items; ++it) {
    if( L1items[it] != "" ) {
      ATH_MSG_DEBUG("L1Item " << it << " " << L1items[it] );
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
  ATH_MSG_DEBUG( "HLT chain monitoring" );

  //Set up the regex map
  std::map<std::string,std::string> streams;
  streams.insert(std::make_pair("HLT_AllChains", "HLT_.*"));
  streams.insert(std::make_pair("HLT_Electrons", "HLT_[0-9]*e[0-9]+.*"));
  streams.insert(std::make_pair("HLT_Gamma",     "HLT_[0-9]*g[0-9]+.*"));
  streams.insert(std::make_pair("HLT_Muons",     "HLT_[0-9]*mu[0-9]+.*"));
  streams.insert(std::make_pair("HLT_Taus",      "HLT_(tau[0-9]*|trk.*Tau).*"));
  streams.insert(std::make_pair("HLT_MissingET", "HLT_(t|x)e[0-9]+.*")); 
  streams.insert(std::make_pair("HLT_Jets",      "HLT_[0-9]*j[0-9]+.*"));
  streams.insert(std::make_pair("HLT_MinBias",   "HLT_mb.*"));

		 
  //// Set the values of the monitored variables for the event	

  //Number of RoIs per signature	
  std::vector<int> roiN;

  //Iterate over the regex map...
  std::map<std::string,std::string>::const_iterator strItr;
  int N_sig = 0;
  for (strItr=streams.begin();strItr!=streams.end(); ++strItr){     
    std::string signaturename = strItr->first;
    std::string thisregex = strItr->second;

    //Open the right group
    std::string toolname = "Trig"+signaturename+"Monitor";
    ATH_MSG_DEBUG("Initializing tool " << toolname );
    auto thisTool = getGroup(toolname);

    //RAW and PS
    std::string histname_raw = signaturename+"RAW";
    std::string histname_ps = signaturename+"PS";
    auto HLT_RAW = Monitored::Scalar<std::string>(histname_raw);
    auto HLT_PS  = Monitored::Scalar<std::string>(histname_ps);

    //eta and phi
    std::string histname_eta=signaturename+"_eta";
    std::string histname_phi=signaturename+"_phi";
    auto RoI_eta = Monitored::Scalar(histname_eta,0.0);
    auto RoI_phi = Monitored::Scalar(histname_phi,0.0);


    //Number of RoIs
    roiN.push_back(0); //initialize roiN at 0 for each signature
    std::string histname_roiN=signaturename+"RoI_N";
    auto RoI_N = Monitored::Scalar(histname_roiN,0);

    //Loop over HLT chains
    ATH_MSG_DEBUG( "Filling HLT" << signaturename << " and RoI information for " << thisregex );

    std::vector< std::string > chainNames = m_trigDecTool->getChainGroup(thisregex)->getListOfTriggers();
    unsigned int nHLTChains = chainNames.size();

    for(unsigned int ith=0; ith<nHLTChains; ++ith) {
      if( chainNames[ith] != "" ) {

	ATH_MSG_DEBUG("HLTChain " << ith << " " << chainNames[ith] );  
	if(m_trigDecTool->isPassed(chainNames[ith], TrigDefs::requireDecision)) {
	  ATH_MSG_DEBUG( "    Chain " << chainNames[ith] << " IS passed");  

	  /// Fill plain chains histogram
	  HLT_RAW = chainNames[ith];
	  fill(tool,HLT_RAW);
	
	  //If the chain is prescaled
	  const TrigConf::HLTChain* c = m_trigDecTool->ExperimentalAndExpertMethods().getChainConfigurationDetails(chainNames[ith]);
	  float prescale = 0;
	  if (c) {
	    prescale = c->prescale();
	  }
	  else {
	    ATH_MSG_WARNING("No chain found in m_trigDecTool->ExperimentalAndExpertMethods().getChainConfigurationDetails(" <<  chainNames[ith] << "). Using prescale 0");
	  }
	  if(prescale>1. || prescale<1.) {
	    //NB! Right now very few chains are prescaled, so this histogram is seldom filled
	    HLT_PS = chainNames[ith];
	    ATH_MSG_DEBUG( "HLT_PS: " << chainNames[ith] << " has PS = " << prescale); 
	    fill(tool,HLT_PS);
	  }

	  /// Fill RoIs histogram and 1D histos for eta, phi, RoI count
	  std::vector<LinkInfo<TrigRoiDescriptorCollection>> fvec = m_trigDecTool->features<TrigRoiDescriptorCollection>(chainNames[ith], TrigDefs::Physics, "", TrigDefs::lastFeatureOfType, initialRoIString()); 

	  //Loop over RoIs
	  for (const LinkInfo<TrigRoiDescriptorCollection>& li : fvec) {
	    if( li.isValid() ) {

	      //Fill 1D histos of roi_N, eta, phi
	      const TrigRoiDescriptor* roi = *(li.link).cptr();
	      if(!roi->isFullscan()) {
		RoI_eta = roi->eta();
		RoI_phi = roi->phi();
		fill(tool,RoI_eta);
		fill(tool,RoI_phi);
		roiN[N_sig]++;
	      }
	      else {
		ATH_MSG_DEBUG( "RoI is FULLSCAN, chain " << chainNames[ith]); 
	      }

	      //Fill 2D RoI maps
	      auto phi = Monitored::Scalar("phi",0.0);
	      auto eta = Monitored::Scalar("eta",0.0);
	      
	      if(!roi->isFullscan()) {
		auto HLT_RoIs = Monitored::Group(thisTool, eta, phi);
		const TrigRoiDescriptor* roi = *(li.link).cptr();
		eta = roi->eta();
		phi = roi->phi();
	      }
	    }//end if(li.isValid())

	    else {
	      ATH_MSG_WARNING( "TrigRoiDescriptorCollection for chain " << chainNames[ith] << " is not valid");
	    }

	  }//end for (const LinkInfo<TrigRoiDescriptorCollection>& li : fvec)
	}// end if(m_trigDecTool->isPassed(chainNames[ith]))
      }// end if( chainNames[ith] != "" )
    }//end for(unsigned int ith=0; ith<nHLTChains; ++ith)

    //Fill RoI count per stream
    RoI_N = roiN[N_sig];
    if(roiN[N_sig]>0) {//only fill if we have at least one RoI for the signature
      fill(tool,RoI_N);
    }

    N_sig++;
  }//end loop over streams
 

  //////////////////////////////////////
  // HLTResult and ConfigConsistency
  
  sc = fillResultAndConsistencyHistograms(ctx);
  
  
  //////////////////////////////////////
  // End
  ATH_MSG_DEBUG( "Finalizing the TrigHLTMonitorAlgorithm..." );
  return StatusCode::SUCCESS;
}


StatusCode TrigHLTMonitorAlgorithm::fillResultAndConsistencyHistograms( const EventContext& ctx ) const {

  using namespace Monitored;
  StatusCode sc_hltEvents = StatusCode::FAILURE; 
  StatusCode sc_onlineKeys = StatusCode::FAILURE; 
  StatusCode sc_eventKeys = StatusCode::FAILURE; 
  ATH_MSG_DEBUG("Filling Result and Consistency histograms");

  // Declare the quantities which should be monitored
  //NB! The variables and histograms defined here must match the ones in the py file exactly!
  auto HLTEvents = Monitored::Scalar<int>("HLTEvents",0);
  auto ConfigConsistency_HLT = Monitored::Scalar<int>("ConfigConsistency_HLT",0);

  SG::ReadHandle<xAOD::TrigConfKeys> onlineKeys(m_onlineKey, ctx);
  SG::ReadHandle<xAOD::TrigConfKeys> eventKeys(m_eventKey, ctx);

  sc_hltEvents = StatusCode::SUCCESS; //This is just a counter variable. If we are executing this code, we fill it. 

  if( onlineKeys.isValid() ) {
    sc_onlineKeys = StatusCode::SUCCESS;
    ATH_MSG_DEBUG("onlineKeys are valid"); 
  }
  else {
    ATH_MSG_ERROR("TrigConfKeysOnline not available");
  }
  if( eventKeys.isValid() ) {
    sc_eventKeys = StatusCode::SUCCESS;
    ATH_MSG_DEBUG("eventKeys are valid");
  }
  else {
    ATH_MSG_ERROR("TrigConfKeys not available");
  }

  //////////////////////////////////////
  // HLTResult and ConfigConsistency

  uint32_t bskeys_1 = 9999; uint32_t bskeys_2 = 9999;
  
  // Fill. First argument is the tool (GMT) name as defined in the py file, 
  // all others are the variables to be saved.
  // Alternative fill method. Get the group yourself, and pass it to the fill function.
  auto tool = getGroup("TrigHLTMonitor");

  ATH_MSG_DEBUG("Fetching keys from TrigConfKeysOnline");     
  if(sc_onlineKeys == StatusCode::SUCCESS) {
    bskeys_1 = onlineKeys->smk();
    bskeys_2 = onlineKeys->hltpsk();
    ATH_MSG_DEBUG("TrigConfKeysOnline: SMK = bskeys_1 = " << bskeys_1 << ", HLTPSK = bskeys_2 = " << bskeys_2);
  }
  else {
    ATH_MSG_WARNING("===> No online keys");
    ConfigConsistency_HLT=7;
    fill(tool,ConfigConsistency_HLT); //Fills the first warning bin in the ConfigConsistency histogram
    bskeys_1 = 0;
    bskeys_2 = 0;
    ATH_MSG_DEBUG("No online keys, reverting to default 0: SMK = bskeys_1 = " << bskeys_1 << ", HLTPSK = bskeys_2 = " << bskeys_2);
  }

  ATH_MSG_DEBUG("===> Filling ConfigConsistency_HLT");

  uint32_t bskeys[] = {bskeys_1, bskeys_2};
  uint32_t dbkeys[2];
  ATH_MSG_DEBUG("Fetching keys from TrigConfKeys");
  if(sc_eventKeys == StatusCode::SUCCESS) {
    dbkeys[0] = eventKeys->smk();
    dbkeys[1] = eventKeys->hltpsk();

    ATH_MSG_DEBUG("TrigConfKeys: SMK = dbkeys[0] = " << dbkeys[0] << ", HLTPSK = dbkeys[1] = " << dbkeys[1]);
  }
  else {
    ATH_MSG_WARNING("===> No event keys");
    ConfigConsistency_HLT=8;
    fill(tool,ConfigConsistency_HLT); //Fills the second warning bin in the ConfigConsistency histogram
    dbkeys[0] = 0;
    dbkeys[1] = 0;
    ATH_MSG_DEBUG("No event keys, reverting to default 0: SMK = dbkeys[0] = " << dbkeys[0] << ", HLTPSK = dbkeys[1] = " << dbkeys[0]);
  }

  for(int i = 0; i < 2; ++i) {
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



  //Fill HLTEvent histogram 
  //this was called HLTResult in Run1-2, 
  //but HLTResult has a slightly different meaning now
  HLTEvents = (int)sc_hltEvents.isSuccess();
  fill(tool,HLTEvents); //Always fill, for every event

  return StatusCode::SUCCESS;
}


