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

  //Fetch the tools 
  auto tool = getGroup("TrigHLTMonitor");
  auto toolAll = getGroup("TrigHLTAllMonitor");
  auto toolEle = getGroup("TrigHLTEleMonitor");
  auto toolGam = getGroup("TrigHLTGamMonitor");
  auto toolMuo = getGroup("TrigHLTMuoMonitor");
  auto toolMET = getGroup("TrigHLTMETMonitor");
  auto toolTau = getGroup("TrigHLTTauMonitor");
  auto toolJet = getGroup("TrigHLTJetMonitor");


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

  ATH_MSG_DEBUG( "Setting up the regex map..." );
  std::map<std::string,std::string> streams;
  streams.insert(std::make_pair("HLT_AllChains", "HLT_.*"));
  streams.insert(std::make_pair("HLT_Electrons", "HLT_[0-9]*e[0-9]+.*"));
  streams.insert(std::make_pair("HLT_Gamma",     "HLT_[0-9]*g[0-9]+.*"));
  streams.insert(std::make_pair("HLT_Muons",     "HLT_[0-9]*mu[0-9]+.*"));
  streams.insert(std::make_pair("HLT_MissingET", "HLT_(t|x)e[0-9]+.*")); 
  streams.insert(std::make_pair("HLT_Taus",      "HLT_(tau[0-9]*|trk.*Tau).*"));
  streams.insert(std::make_pair("HLT_Jets",      "HLT_[0-9]*j[0-9]+.*"));
  streams.insert(std::make_pair("HLT_MinBias",   "HLT_mb.*"));

		 
  //// Set the values of the monitored variables for the event		

  ATH_MSG_DEBUG( "Iterating over the regex map...");
  std::map<std::string,std::string>::const_iterator strItr;
  for (strItr=streams.begin();strItr!=streams.end(); ++strItr){     
    std::string signaturename = strItr->first;
    std::string thisregex = strItr->second;
    std::string histname_raw = signaturename+"RAW";
    std::string histname_ps = signaturename+"PS";
    auto HLT_RAW = Monitored::Scalar<std::string>(histname_raw);
    auto HLT_PS  = Monitored::Scalar<std::string>(histname_ps);
    ATH_MSG_DEBUG( "Filling HLT" << signaturename << " and RoI information for " << thisregex );

    std::vector< std::string > chainNames = m_trigDecTool->getChainGroup(thisregex)->getListOfTriggers();
    unsigned int nHLTChains = chainNames.size();

    for(unsigned int ith=0; ith<nHLTChains; ++ith) {
      if( chainNames[ith] != "" ) {

	ATH_MSG_DEBUG("HLTChain " << ith << " " << chainNames[ith] );
	if(m_trigDecTool->isPassed(chainNames[ith])) {
	  ATH_MSG_DEBUG( "    Chain " << chainNames[ith] << " IS passed");

	  /// Fill plain chains histogram
	  HLT_RAW = chainNames[ith];
	  ATH_MSG_DEBUG( "Fill HLT_RAW for " << signaturename << " and " << chainNames[ith]); 
	  fill(tool,HLT_RAW);
	
	  //If the chain is prescaled
	  ATH_MSG_DEBUG( "Prescale: " << m_trigDecTool->getPrescale(chainNames[ith]) );
	  if(m_trigDecTool->getPrescale(chainNames[ith])!=1) {
	    //NB! Right now very few chains are prescaled, so this histogram is seldom filled
	    HLT_PS = chainNames[ith];
	    ATH_MSG_DEBUG( "Fill HLT_PS for " << signaturename << " and " << chainNames[ith]); 
	    fill(tool,HLT_PS);
	  }

	  /// Fill RoIs histogram
	  ATH_MSG_DEBUG("Fill RoI histograms for chain " << chainNames[ith] );
	  std::vector<LinkInfo<TrigRoiDescriptorCollection>> fvec = m_trigDecTool->features<TrigRoiDescriptorCollection>(chainNames[ith], TrigDefs::Physics, "", TrigDefs::lastFeatureOfType, initialRoIString()); 


	  //Loop over RoIs
	  for (const LinkInfo<TrigRoiDescriptorCollection>& li : fvec) {
	    if( li.isValid() ) {
	      auto phi = Monitored::Scalar("phi",0.0);
	      auto eta = Monitored::Scalar("eta",0.0);
	      if(signaturename=="HLT_AllChains") {
		ATH_MSG_DEBUG( "RoI: filling for " << signaturename );
		auto HLT_RoIs = Monitored::Group(toolAll, eta, phi);
		const TrigRoiDescriptor* roi = *(li.link).cptr();
		eta = roi->eta();
		phi = roi->phi();
		ATH_MSG_DEBUG( "RoI: eta = " << eta << ", phi = " << phi ); 
	      }

	      //Check signatures
	      if(signaturename=="HLT_Electrons") {
		ATH_MSG_DEBUG( "RoI: filling for " << signaturename );
		auto HLT_RoIs = Monitored::Group(toolEle, eta, phi);
		const TrigRoiDescriptor* roi = *(li.link).cptr();
		eta = roi->eta();
		phi = roi->phi();
		ATH_MSG_DEBUG( "RoI: eta = " << eta << ", phi = " << phi ); 
	      }
	      else if(signaturename=="HLT_Gamma") {
		ATH_MSG_DEBUG( "RoI: filling for " << signaturename );
		auto HLT_RoIs = Monitored::Group(toolGam, eta, phi);
		const TrigRoiDescriptor* roi = *(li.link).cptr();
		eta = roi->eta();
		phi = roi->phi();
		ATH_MSG_DEBUG( "RoI: eta = " << eta << ", phi = " << phi ); 
	      }
	      else if(signaturename=="HLT_Muons") {
		ATH_MSG_DEBUG( "RoI: filling for " << signaturename );
		auto HLT_RoIs = Monitored::Group(toolMuo, eta, phi);
		const TrigRoiDescriptor* roi = *(li.link).cptr();
		eta = roi->eta();
		phi = roi->phi();
		ATH_MSG_DEBUG( "RoI: eta = " << eta << ", phi = " << phi ); 
	      }
	      else if(signaturename=="HLT_MissingET") {
		ATH_MSG_DEBUG( "RoI: filling for " << signaturename );
		auto HLT_RoIs = Monitored::Group(toolMET, eta, phi);
		const TrigRoiDescriptor* roi = *(li.link).cptr();
		eta = roi->eta();
		phi = roi->phi();
		ATH_MSG_DEBUG( "RoI: eta = " << eta << ", phi = " << phi ); 
	      }
	      else if(signaturename=="HLT_Taus") {
		ATH_MSG_DEBUG( "RoI: filling for " << signaturename );
		auto HLT_RoIs = Monitored::Group(toolTau, eta, phi);
		const TrigRoiDescriptor* roi = *(li.link).cptr();
		eta = roi->eta();
		phi = roi->phi();
		ATH_MSG_DEBUG( "RoI: eta = " << eta << ", phi = " << phi ); 
	      }
	      else if(signaturename=="HLT_Jets") {
		ATH_MSG_DEBUG( "RoI: filling for " << signaturename );
		auto HLT_RoIs = Monitored::Group(toolJet, eta, phi);
		const TrigRoiDescriptor* roi = *(li.link).cptr();
		eta = roi->eta();
		phi = roi->phi();
		ATH_MSG_DEBUG( "RoI: eta = " << eta << ", phi = " << phi ); 
	      }



	    }//end if(li.isValid())

	    else {
	      ATH_MSG_WARNING( "TrigRoiDescriptorCollection for chain " << chainNames[ith] << " is not valid");
	    }

	  }//end for (const LinkInfo<TrigRoiDescriptorCollection>& li : fvec)
	}// end if(m_trigDecTool->isPassed(chainNames[ith]))
      }// end if( chainNames[ith] != "" )
    }//end for(unsigned int ith=0; ith<nHLTChains; ++ith)
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
  ATH_MSG_DEBUG("---> Filling Result and Consistency histograms");

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


