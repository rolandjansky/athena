/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile HLTMuonMonTool.cxx
 *   
 *    authors: Andrea Ventura (ventura@le.infn.it)
 *             Margherita Primavera (margherita.primavera@le.infn.it)                       
 *             Akimasa Ishikawa (akimasa.ishikawa@cern.ch)             
 *             Stefano Giagu (stefano.giagu@cern.ch)    
 *             Antonio Sidoti (antonio.sidoti@roma1.infn.it)    
 *             Heuijin Lim (hjlim@hep.anl.gov)
 *             Kunihiro Nagano (nagano@mail.cern.ch)
 *             Arantxa Ruiz (aranzazu.ruiz.martinez@cern.ch)
 */

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include <iostream>

#include "TrigMuonMonitoring/HLTMuonMonTool.h"

//for GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

//YY: for MuonCombinedSelector tool - not to use for the moment
// #include "MuonCombinedToolInterfaces/IMuonCombinedSelectorTool.h"


using namespace std;

//main DQA routine
// DQA for each HLT Algorithms are found after the main routine


/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selectio
 *  of the filepath for histograms, the first and second plane
 *  numbers to be used, and the timebin.
 */
HLTMuonMonTool::HLTMuonMonTool(const std::string & type, 
                               const std::string & name,
                               const IInterface* parent)
  //initialization of common parameters
  :IHLTMonTool(type, name, parent),
   m_ExpertMethods(0),
   m_nullstr("null"),  // yy added
   m_bunchTool("Trig::TrigConfBunchCrossingTool/BunchCrossingTool")
   //   m_muonSelectorTool("Rec::MuonSelectorTool") // YY added -> removed
   
  //initialization of L2MuonSA parameters

  //initialization of muComb parameters

  //initialization of muIso parameters

  //initialization of TileMu parameters

  //initialization of MuonEF parameters

  //initialization of MuGirl parameters

  //initialization of MuZTP parameters

{
  ATH_MSG_VERBOSE(" construction of HLTMuonMonTool");
  //construction of common parameters
  //declareProperty("foobar",     m_foobar=false);
  //  declareProperty("MuonSelectorTool",m_muonSelectorTool);  // YY added -> removed
  declareProperty("monitoring_muonNonIso", m_chainsGeneric);
  declareProperty("monitoring_muonIso", m_chainsEFiso);
  declareProperty("monitoring_MSonly", m_chainsMSonly);
  declareProperty("monitoring_muonEFFS", m_chainsEFFS);
  declareProperty("monitoring_muonLowpt", m_chainsLowpt);
  declareProperty("monitoring_muon_Support", m_chainSupport);
  declareProperty("HI_pp_mode", m_HI_pp_mode);

  declareProperty("AccessHypoTEs", m_access_hypoTE=true);
  //construction of L2MuonSA parameters
  declareProperty("monitoring_muonNonIso_L2SAHypo", m_hyposGeneric_L2SA);
  declareProperty("monitoring_muonIso_L2SAHypo", m_hyposEFiso_L2SA);
  declareProperty("monitoring_MSonly_L2SAHypo", m_hyposMSonly_L2SA);
  declareProperty("monitoring_muonEFFS_L2SAHypo", m_hyposEFFS_L2SA);
  declareProperty("monitoring_muonLowpt_L2SAHypo", m_hyposLowpt_L2SA);
  declareProperty("monitoring_muon_Support_L2SAHypo", m_hyposSupport_L2SA);

  //construction of muComb parameters
  declareProperty("monitoring_muonNonIso_L2CBHypo", m_hyposGeneric_L2CB);
  declareProperty("monitoring_muonIso_L2CBHypo", m_hyposEFiso_L2CB);
  declareProperty("monitoring_MSonly_L2CBHypo", m_hyposMSonly_L2CB);
  declareProperty("monitoring_muonEFFS_L2CBHypo", m_hyposEFFS_L2CB);
  declareProperty("monitoring_muonLowpt_L2CBHypo", m_hyposLowpt_L2CB);
  declareProperty("monitoring_muon_Support_L2CBHypo", m_hyposSupport_L2CB);


  //construction of muIso parameters

  //construction of TileMu parameters

  //construction of MuonEF parameters
  declareProperty("DeltaRCut", m_deltaRCut = 0.15);

  //construction of MuGirl parameters

  //construction of MuZTP parameters
  //declareProperty("ChainsForZTP", m_ztp_isomap);
  declareProperty("ZTPPtCone30RelCut",m_ztp_ptcone30rel_cut=0.06);
  declareProperty("ZTP_EFPtCone30RelCut",m_ztp_EF_ptcone30rel_cut=0.12);
  declareProperty("BCTool", m_bunchTool);
  m_activeStore = 0;
  m_lumiblock = 0;
  m_event = 0;
  m_maxindep = 0;
  m_maxESbr = 0;
  m_requestESchains = 0;
  m_fL2MuonSA = 0;
  m_fMuComb = 0;
  m_fEFCB = 0;
  m_fMuGirl = 0;
  m_fEFSA = 0;
  m_iSTDL = 0;
  m_iSTDH = 0;
  m_iMSL = 0;
  m_iMSH = 0;
  m_ztp_newrun = 0;
  
}

/*---------------------------------------------------------*/
HLTMuonMonTool::~HLTMuonMonTool()
  /*---------------------------------------------------------*/
{
  ATH_MSG_VERBOSE(" destruction of HLTMuonMonTool");
  //destruction of common parameters

  //destruction of L2MuonSA parameters

  //destruction of muComb parameters

  //destruction of muIso parameters

  //destruction of TileMu parameters

  //destruction of MuonEF parameters

  //destruction of MuGirl parameters

  //destruction of MuZTP parameters

}

/*---------------------------------------------------------*/
StatusCode HLTMuonMonTool::init()
  /*---------------------------------------------------------*/
{
  //(*m_log).setLevel(MSG::DEBUG); // YY: not yet used
  // this->msg().setLevel(MSG::DEBUG);  // YY tried this, worked fine
  ATH_MSG_DEBUG("init being called");
  ATH_MSG_DEBUG("HI_pp_mode:" << m_HI_pp_mode);
  // some switches and flags
  m_requestESchains = true;
  //initialization for common tools
  StatusCode scAS;

  scAS = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (scAS != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR(" Cannot get ActiveStoreSvc ");
    return scAS ;
  }

  CHECK(m_bunchTool.retrieve());

  /*  not to use
  if ( m_muonSelectorTool.empty() ) {
    ATH_MSG_ERROR(m_muonSelectorTool << " not configured" );
    return StatusCode::FAILURE;
  } 
  StatusCode scc = m_muonSelectorTool.retrieve();
  if (scc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_muonSelectorTool );
    return StatusCode::FAILURE;
  }

  */

  m_ExpertMethods = getTDT()->ExperimentalAndExpertMethods();
  if (m_ExpertMethods) m_ExpertMethods->enable();
  else ATH_MSG_ERROR("No expert Methods for Trigger Descision Tool are found!");

  // chainAnalysis chains

  // Generic (EFsuper etc.)
  // m_chainsGeneric.push_back("mu24_medium"); // for testing 22.02.12
  // m_chainsGeneric.push_back("mu24_MG_medium"); // for testing 22.02.12
  //  m_chainsGeneric.push_back("mu36_tight");             // v4 primary
  //
  //m_chainsGeneric.push_back("mu4_cosmic_L1MU4_EMPTY");   // LS1 sept25
  //m_chainsGeneric.push_back("mu4_cosmic_L1MU11_EMPTY");  // LS1 sept25
  //m_chainsGeneric.push_back("mu24_imedium"); // sept25
  //m_chainsGeneric.push_back("mu26_imedium"); // sept25
  //m_chainsGeneric.push_back("mu50"); // sept25
  for(unsigned int ich = 0; ich < m_chainsGeneric.size(); ich++){
	m_histChainGeneric.push_back("muChain"+std::to_string(ich+1));
	m_ztp_isomap.insert(std::pair<std::string, int>(m_chainsGeneric[ich], 0));
	m_ztpmap.insert(std::pair<std::string, std::string>(m_chainsGeneric[ich], "muChain"+std::to_string(ich+1)));
	if(ich<m_hyposGeneric_L2SA.size()) m_hypomapL2SA[m_chainsGeneric[ich]] = m_hyposGeneric_L2SA[ich];
	if(ich<m_hyposGeneric_L2CB.size()) m_hypomapL2CB[m_chainsGeneric[ich]] = m_hyposGeneric_L2CB[ich];
  }
  
  // Generic (Isolated muons)
  // m_chainsEFiso.push_back("mu20i"); // for testing 22.02.12
  // m_chainsEFiso.push_back("mu24i_tight")  ;            // v4 primary

  for(unsigned int ich = 0; ich < m_chainsEFiso.size(); ich++){
	m_histChainEFiso.push_back("muChainEFiso"+std::to_string(ich+1));
	m_ztp_isomap.insert(std::pair<std::string, int>(m_chainsEFiso[ich], 1));
	m_ztpmap.insert(std::pair<std::string, std::string>(m_chainsEFiso[ich], "muChainEFiso"+std::to_string(ich+1)));
	if(ich<m_hyposEFiso_L2SA.size()) m_hypomapL2SA[m_chainsEFiso[ich]] = m_hyposEFiso_L2SA[ich];
	if(ich<m_hyposEFiso_L2CB.size()) m_hypomapL2CB[m_chainsEFiso[ich]] = m_hyposEFiso_L2CB[ich];
  }
 

  // MSonly
  // m_chainsMSonly.push_back("mu40_MSonly_barrel"); // YY for 2011, _tight may have to be monitored too
  // m_chainsMSonly.push_back("mu40_MSonly_barrel_medium"); // YY for 2e33, added
  // m_chainsMSonly.push_back("mu50_MSonly_barrel_tight");    // v4 primary
  //m_chainsMSonly.push_back("mu4_msonly_cosmic_L1MU11_EMPTY");    // LS1
  //
  for(unsigned int ich = 0; ich < m_chainsMSonly.size(); ich++){
	m_histChainMSonly.push_back("muChainMSonly"+std::to_string(ich+1));
	m_ztp_isomap.insert(std::pair<std::string, int>(m_chainsMSonly[ich], 0));
	m_ztpmap.insert(std::pair<std::string, std::string>(m_chainsMSonly[ich], "muChainMSonly"+std::to_string(ich+1)));
	if(ich<m_hyposMSonly_L2SA.size()) m_hypomapL2SA[m_chainsMSonly[ich]] = m_hyposMSonly_L2SA[ich];
	if(ich<m_hyposMSonly_L2CB.size()) m_hypomapL2CB[m_chainsMSonly[ich]] = m_hyposMSonly_L2CB[ich];
  }


  // EFFS triggers (L. Yuan)
  
  // Standard
  // m_chainsStandard.push_back("mu4");
  // m_chainsStandard.push_back("mu4T");
  // m_chainsStandard.push_back("mu6");
  // m_chainsStandard.push_back("mu4_L1VTE50"); // for HI run, the only primary chain with combiners
  // m_chainsStandard.push_back("mu20");
  // m_chainsStandard.push_back("mu22_medium");  // YY added 26.06.2011, replaced to _medium on 01 Aug 2011 for 2e33

  // v4 primary
  // m_chainsStandard.push_back("mu18_tight_mu8_EFFS");  // YY added 26.06.2011, replaced to _medium on 01 Aug 2011 for 2e33
  // m_FS_pre_trigger = "mu18it_tight";
  // m_FS_pre_trigger_second = "mu24i_tight";
  //
  // v5 primary
  //m_histChainEFFS.push_back("muChainEFFS");
  //m_chainsEFFS.push_back("mu18_mu8noL1");

  if(!m_HI_pp_mode){
    m_FS_pre_trigger = "HLT_mu4";
    m_FS_pre_trigger_second = "HLT_mu8";
  }else{
    m_FS_pre_trigger = "HLT_mu24_ivarmedium";
    m_FS_pre_trigger_second = "HLT_mu26_ivarmedium";
  }

  for(unsigned int ich = 0; ich < m_chainsEFFS.size(); ich++){
	if(ich > 0) continue;
	m_histChainEFFS.push_back("muChainEFFS");
  }

  
  // chainAnalysis for MuGirl
  // Warning: corresponding standard chain has to exist!!!!!!!!
  // m_chainsMG.push_back("mu6_MG");
  // m_chainsMG.push_back("mu20_MG"); 
  // m_chainsMG.push_back("mu22_MG_medium"); // YY replaced to _medium on 01 Aug 2011 for 2e33

  //// chainAnalysis for MuIso
  // m_chainsMI.push_back("mu10i_loose");
  // m_chainsMI.push_back("mu15i");
  // m_chainsMI.push_back("mu20i_medium"); // YY added: _medium on 01 Aug 2011 for 2e33

  // used in muComb 
  m_chainsL2Standard.push_back("L2_mu18_tight");
  m_chainsL2Standard.push_back("L2_mu24_tight");
  m_chainsL2Standard.push_back("L2_mu36_tight");
  m_chainsL2Standard.push_back("L2_mu10");
  m_chainsL2Standard.push_back("L2_mu13");
  
  m_thresh["mu4_MSonly"] = 4;
  m_thresh["mu4T_MSonly"] = 4;
  m_thresh["mu4_MSonly_L1TE50"] = 4;
  m_thresh["mu4T_MSonly_L1TE50"] = 4;
  m_thresh["mu10_MSonly_EFFS_L1ZDC"] = 10; //not in physics_1e31, but for HI runs, full-scan chain
  m_thresh["mu13_MSonly_EFFS_L1ZDC"] = 13; //not in physics_1e31, but for HI runs, full-scan chain
  m_thresh["mu40_MSonly_barrel"] = 40;
  m_thresh["mu40_MSonly_barrel_medium"] = 40; // YY added: _medium on 01 Aug 2011 for 2e33
  m_thresh["mu40_MSonly_barrel_tight"] = 40;
  m_thresh["mu50_MSonly_barrel_tight"] = 50;

  // used in Standard chain monitoring: no more in use
  m_thresh["mu4"] = 4;
  m_thresh["mu4T"] = 4;
  m_thresh["mu4_L1VTE50"] = 4;
  m_thresh["mu6"] = 6;
  m_thresh["mu10"] = 10;
  m_thresh["mu13"] = 13;
  m_thresh["mu20"] = 20;
  // m_thresh["mu22"] = 22;  // YY added 26.06.2011
  m_thresh["mu22_medium"] = 22;  // YY replaced: _medium on 01 Aug 2011 for 2e33
  // m_thresh["mu40"] = 40;
  // m_thresh["mu10i_loose"] = 10;


  //trigger rate monitored chains v4
  m_chainsRate.push_back("HLT_mu24_tight");  
  m_chainsRate.push_back("HLT_mu24_tight_MG");  
  m_chainsRate.push_back("HLT_mu24_tight_MuonEF");  
  m_chainsRate.push_back("HLT_mu24i_tight");  
  m_chainsRate.push_back("HLT_mu36_tight");  
  m_chainsRate.push_back("HLT_mu50_MSonly_barrel_tight"); 


  //chains for overlap counting
  //MU0/mu4
  m_chainsOverlap.push_back("HLT_mu4_MSonly");
  m_chainsOverlap.push_back("HLT_mu4");
  m_chainsOverlap.push_back("HLT_mu4_MG");
  m_chainsOverlap.push_back("HLT_mu4_tile");
  m_chainsOverlap.push_back("HLT_mu4_muCombTag");
  m_chainsOverlap.push_back("HLT_mu4_MSonly_EFFS_passL2");
  m_chainsOverlap.push_back("HLT_mu4_L2MSonly_EFFS_passL2");

  //MU6/mu6
  m_chainsOverlap.push_back("HLT_mu6_MSonly");
  m_chainsOverlap.push_back("HLT_mu6");
  m_chainsOverlap.push_back("HLT_mu6_MG");
  m_chainsOverlap.push_back("HLT_mu6_muCombTag");

  //MU10/mu10
  m_chainsOverlap.push_back("HLT_mu10_MSonly");
  m_chainsOverlap.push_back("HLT_mu10_MSonly_tight");
  m_chainsOverlap.push_back("HLT_mu10");
  m_chainsOverlap.push_back("HLT_mu10_MG");
  m_chainsOverlap.push_back("HLT_mu10i_loose");

  //mu13
  m_chainsOverlap.push_back("HLT_mu13");

  //MU11

  //MU15/mu15
  m_chainsOverlap.push_back("HLT_mu15_NoAlg");
  m_chainsOverlap.push_back("HLT_mu15");

  //MU20/mu20
  m_chainsOverlap.push_back("HLT_mu20_NoAlg");
  m_chainsOverlap.push_back("HLT_mu20_passHLT");
  m_chainsOverlap.push_back("HLT_mu20_MSonly");
  m_chainsOverlap.push_back("HLT_mu20");
  m_chainsOverlap.push_back("HLT_mu20_slow");

  //mu30
  m_chainsOverlap.push_back("HLT_mu30_MSonly");

  // dimuon
  m_chainsOverlap.push_back("HLT_2mu4");
  m_chainsOverlap.push_back("HLT_2MUL1_j40_HV");
  m_chainsOverlap.push_back("HLT_mu4_mu6");
  m_chainsOverlap.push_back("HLT_2mu6");
  m_chainsOverlap.push_back("HLT_2mu10");

  //trigger rate in events taken by indepedent triggers
  m_chains2.push_back("L2_mu4");
  m_chains2.push_back("L2_mu6");
  m_chains2.push_back("L2_mu10");
  m_chains2.push_back("L2_mu13");
  m_chains2.push_back("L2_mu15");
  m_chains2.push_back("L2_mu20");
  m_chains2.push_back("L2_mu40");
  m_chains2.push_back("L2_mu20_MSonly");

  m_chains2.push_back("EF_mu4");
  m_chains2.push_back("EF_mu6");
  m_chains2.push_back("EF_mu10");
  m_chains2.push_back("EF_mu13");
  m_chains2.push_back("EF_mu15");
  m_chains2.push_back("EF_mu20");
  m_chains2.push_back("EF_mu40");
  m_chains2.push_back("EF_mu20_MSonly");

 //added by marx - to be editted (YY 2e33)
  /* m_ztpmap["mu15"]="L1_MU10"; 
  m_ztpmap["mu15i"]="L1_MU10"; 
  m_ztpmap["mu20i_medium"]="L1_MU11"; 
  m_ztpmap["mu20"]="L1_MU10"; 
  m_ztpmap["mu20_MG"]="L1_MU10";
  m_ztpmap["mu22_medium"]="L1_MU11"; // YY
  m_ztpmap["mu22_MG_medium"]="L1_MU11";
  m_ztpmap["mu40_MSonly_barrel"]="L1_MU10";
  m_ztpmap["mu40_MSonly_barrel_medium"]="L1_MU11";
  // m_ztpmap["mu40_MSonly_tight"]="L1_MU10";   // YY commented out 26.06.2011
  // m_ztpmap["mu40_MSonly_tighter"]="L1_MU10";  // YY commented out 26.06.2011
  m_ztpptcut["mu15"]=17.;
  m_ztpptcut["mu15i"]=17.;
  m_ztpptcut["mu20i_medium"]=22.;
  m_ztpptcut["mu20"]=22.;
  m_ztpptcut["mu20_MG"]=22.;
  m_ztpptcut["mu22_medium"]=24.;
  m_ztpptcut["mu22_MG_medium"]=24.;
  m_ztpptcut["mu40_MSonly_barrel"]=42.;
  m_ztpptcut["mu40_MSonly_barrel_medium"]=42.;
  // m_ztpptcut["mu40_MSonly_tight"]=42.;  // YY commented out 26.06.2011
  // m_ztpptcut["mu40_MSonly_tighter"]=42.;  // YY commented out 26.06.2011
  */



  /* m_trigger[0] = "Orthog";//EGamma + Tau + Jet + MET
  m_trigger[1] = "EGamma";
  m_trigger[2] = "Tau";
  m_trigger[3] = "Jet";
  m_trigger[4] = "MET";
  m_trigger[5] = "MinBias"; */
  m_trigger[INDORTH] = "Orthog";//EGamma + Tau + Jet + MET
  m_trigger[INDEGAMMA] = "EGamma";
  m_trigger[INDMET] = "MET";
  m_trigger[INDJET] = "Jet";

  m_maxindep = INDORTH; // loop over i < INDORTH = 1, which means zero iteration!!

  m_maxESbr = ESINDEP;

  m_ESchainName = "_ES";

  m_CB_mon_ESbr[ESSTD] = 0;
  m_CB_mon_ESbr[ESTAG] = 0;
  m_CB_mon_ESbr[ESID] = 1;
  m_CB_mon_ESbr[ESINDEP] = 0;
  m_CB_mon_ESbr[ESHIL1] = 0;
  m_CB_mon_ESbr[ESHIID] = 0;
  m_CB_mon_ESbr[ESHIINDEP] = 0;

  m_MS_mon_ESbr[ESSTD] = 0;
  m_MS_mon_ESbr[ESTAG] = 1;
  m_MS_mon_ESbr[ESID] = 0;
  m_MS_mon_ESbr[ESINDEP] = 0;
  m_MS_mon_ESbr[ESHIL1] = 0;
  m_MS_mon_ESbr[ESHIID] = 0;
  m_MS_mon_ESbr[ESHIINDEP] = 0;

  // ES chain catetory names: updated 15.10.2011
  m_triggerES[ESSTD] = "_ESstd";
  m_triggerES[ESTAG] = "_EStag";
  m_triggerES[ESID] = "_ESid";
  m_triggerES[ESINDEP] = "_ESindep"; // added 20.01.2012
  m_triggerES[ESHIL1] = "_ESHIL1";
  m_triggerES[ESHIID] = "_ESHIid";
  m_triggerES[ESHIINDEP] = "_ESHIindep";
  
  // AI 20100824 - for rate monitoring:
  // Primary chain is mu6
  //m_allESchain.push_back("HLT_mu4");
  //m_allESchain.push_back("HLT_mu6_IDTrkNoCut");
  //m_allESchain.push_back("HLT_2mu4");
  //m_allESchain.push_back("HLT_2mu10");
  //m_allESchain.push_back("HLT_mu10");
  //m_allESchain.push_back("HLT_mu10_MG");  // added 27.10.2010
  //m_allESchain.push_back("HLT_mu20");  // added 27.10.2010
  //m_allESchain.push_back("HLT_mu20_MSonly");

  // Primary chain is mu10
  //m_allESchain.push_back("EF_mu6");
  //m_allESchain.push_back("EF_mu10_IDTrkNoCut");
  //m_allESchain.push_back("EF_2mu4_Jpsimumu");
  //m_allESchain.push_back("EF_2mu4_Upsimumu");
  //m_allESchain.push_back("EF_2mu10"); no duplication
  //m_allESchain.push_back("EF_mu15");
  //m_allESchain.push_back("EF_mu20_MSonly");  no duplication

  // Primary chain is mu13
  //m_allESchain.push_back("EF_mu10"); no duplication
  //m_allESchain.push_back("EF_mu10_IDTrkNoCut");  no duplication
  //m_allESchain.push_back("EF_2mu4_Jpsimumu");  no duplication
  //m_allESchain.push_back("EF_2mu4_Upsimumu");  no duplication
  //m_allESchain.push_back("EF_2mu10"); no duplication
  //m_allESchain.push_back("EF_mu15");  no duplication
  //m_allESchain.push_back("EF_mu30_MSonly");
  
  // Primary chain is mu24_imedium
  m_allESchain.push_back("HLT_mu24_imedium");
  m_allESchain.push_back("HLT_mu18");
  m_allESchain.push_back("HLT_2mu10");
  m_allESchain.push_back("HLT_2mu14");
  m_allESchain.push_back("HLT_mu20_idperf");
  m_allESchain.push_back("HLT_mu6_idperf");
  m_allESchain.push_back("HLT_Zmumu_idperf");
  m_allESchain.push_back("HLT_Jpsimumu_idperf");

  // initialising algorithm index for summary histos
  m_fL2MuonSA = (float)iL2MuonSA;
  m_fMuComb = (float)iMuComb;
  m_fEFCB = (float)iEFCB;
  m_fEFSA = (float)iEFSA;
  m_fMuGirl = (float)iMuGirl;

  m_vectkwd.push_back(m_triggerES[ESTAG]);
  m_vectkwd.push_back(m_triggerES[ESID]);
  m_vectkwd.push_back(m_triggerES[ESINDEP]);
  m_vectkwd.push_back("_Jet");
  m_vectkwd.push_back("_all");

  // YY: pt range.
  if(!m_HI_pp_mode){
    m_iSTDL = 45;  // 40 GeV
    m_iSTDH = 75; // 100 GeV
    m_iMSL = 54;  // 60 GeV
    m_iMSH = 75;  // 100 GeV  
  }else{
    m_iSTDL = 105;//60GeV 91;  // 40 GeV
    m_iSTDH = 120; // 100 GeV
    m_iMSL = 105;  // 60 GeV
    m_iMSH = 120;  // 100 GeV
  /* m_iSTDL = 71;  // 22.5 GeV
  m_iSTDH = 100;  // 50 GeV
  m_iMSL = 91;  // 40 GeV
  m_iMSH = 112;  // 80 GeV */
  }
  // New MSonly_triggered chains: YY added 26.06.2011
  m_MSchainName = "_MSb";  // meaning MSonly_barrel
  // m_MSchain.push_back("EF_mu40_MSonly_barrel");
  // m_MSchain.push_back("EF_mu40_MSonly_barrel_medium");  // YY for 2e33
  // m_MSchain.push_back("EF_mu50_MSonly_barrel_tight");  // v4
  for(unsigned int ich = 0; ich < m_chainsMSonly.size(); ich++){
    m_MSchain.push_back(m_chainsMSonly[ich]);
  }
  m_MSchain.push_back("HLT_mu20_msonly");  // v5 2015
  

  m_hptName = "_hpt";

  m_EFAlgName[0] = "_Combined";
  m_EFAlgName[1] = "_MuonStandAlone";
  m_EFAlgName[2] = "_SegmentTagged";
  m_EFAlgName[3] = "_allEFMuons";

  // 2d phi bins initialisation
  m_bins_eta[0] = -2.40;
  m_bins_eta[1] = -1.918;
  m_bins_eta[2] = -1.623;
  m_bins_eta[3] = -1.348;
  m_bins_eta[4] = -1.2329;
  m_bins_eta[5] = -1.1479;
  m_bins_eta[6] = -1.05;
  m_bins_eta[7] = -0.908;
  m_bins_eta[8] = -0.791;
  m_bins_eta[9] = -0.652;
  m_bins_eta[10] = -0.476;
  m_bins_eta[11] = -0.324;
  m_bins_eta[12] = -0.132;
  m_bins_eta[13] = 0.;
  m_bins_eta[14] = 0.132;
  m_bins_eta[15] = 0.324;
  m_bins_eta[16] = 0.476;
  m_bins_eta[17] = 0.652;
  m_bins_eta[18] = 0.791;
  m_bins_eta[19] = 0.908;
  m_bins_eta[20] = 1.05;
  m_bins_eta[21] = 1.1479;
  m_bins_eta[22] = 1.2329;
  m_bins_eta[23] = 1.348;
  m_bins_eta[24] = 1.623;
  m_bins_eta[25] = 1.918;
  m_bins_eta[26] = 2.40;

  for (int iphi = 0; iphi <= s_phi_cnbins; iphi++) {
    Double_t dphi = TMath::Pi()/8.;
    m_bins_phi[iphi] = (dphi*static_cast<Double_t>(iphi)-(TMath::Pi()));
  }

  //L2MuonSA
  StatusCode scL2MuonSA;
  scL2MuonSA=initL2MuonSADQA();
  if( scL2MuonSA.isFailure() ){
    ATH_MSG_VERBOSE("initL2MuonSADQA failed");
  }
  //muComb
  StatusCode scMuComb;
  scMuComb=initMuCombDQA();
  if( scMuComb.isFailure() ){
    ATH_MSG_VERBOSE("initMuCombDQA failed");
  }
  //muIso
  StatusCode scMuIso;
  scMuIso=initMuIsoDQA();
  if( scMuIso.isFailure() ){
    ATH_MSG_VERBOSE("initMuIsoDQA failed");
  }
  //TileMu
  StatusCode scTileMu;
  scTileMu=initTileMuDQA();
  if( scTileMu.isFailure() ){
    ATH_MSG_VERBOSE("initMuIsoDQA failed");
  }
  //MuonEF
  StatusCode scMuonEF;
  scMuonEF=initMuonEFDQA();
  if( scMuonEF.isFailure() ){
    ATH_MSG_VERBOSE("initMuonEFDQA failed");
  }
  //MuGirl
  StatusCode scMuGirl;
  scMuGirl=initMuGirlDQA();
  if( scMuGirl.isFailure() ){
    ATH_MSG_VERBOSE("initMuGirlDQA failed");
  }
  //MuZTP
  StatusCode scMuZTP;
  scMuZTP=initMuZTPDQA();
  if( scMuZTP.isFailure() ){
    ATH_MSG_VERBOSE("initMuZTPDQA failed");
  }

  StatusCode sc = scL2MuonSA & scMuComb & scMuIso & scTileMu & scMuonEF & scMuGirl & scMuZTP;
  return sc;
  
  // if(sc==1){
  //   return StatusCode::SUCCESS;
  // }else if(sc>1){
  //   return StatusCode::RECOVERABLE;
  // }

  // return StatusCode::FAILURE;
}




/*---------------------------------------------------------*/
StatusCode HLTMuonMonTool::book()
  /*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG("book being called");
  
  m_histdir="HLT/MuonMon/Common";
  m_histdirmufast="HLT/MuonMon/L2MuonSA";
  m_histdirmucomb="HLT/MuonMon/muComb";
  m_histdirmuiso="HLT/MuonMon/muIso";
  m_histdirtilemu="HLT/MuonMon/TileMu";
  m_histdirmuonef="HLT/MuonMon/MuonEF";
  m_histdirmugirl="HLT/MuonMon/MuGirl";
  m_histdirrate="HLT/MuonMon/Rate";
  m_histdirrateratio="HLT/MuonMon/Rate/Ratio";
  m_histdircoverage="HLT/MuonMon/Coverage";
  m_histdireff="HLT/MuonMon/Efficiency";
  m_histdireffnumdenom="HLT/MuonMon/Efficiency/NumDenom";
  m_histdirdist2d="HLT/MuonMon/etaphi2D";
  
  addMonGroup( new MonGroup(this, m_histdir, run, ATTRIB_UNMANAGED) );
  addMonGroup( new MonGroup(this, m_histdirmufast, run, ATTRIB_UNMANAGED) );
  addMonGroup( new MonGroup(this, m_histdirmucomb, run, ATTRIB_UNMANAGED) );
  addMonGroup( new MonGroup(this, m_histdirmuiso, run, ATTRIB_UNMANAGED) );
  addMonGroup( new MonGroup(this, m_histdirtilemu, run, ATTRIB_UNMANAGED) );
  addMonGroup( new MonGroup(this, m_histdirmuonef, run, ATTRIB_UNMANAGED) );
  addMonGroup( new MonGroup(this, m_histdirmugirl, run, ATTRIB_UNMANAGED) );
  addMonGroup( new MonGroup(this, m_histdirrate, run, ATTRIB_UNMANAGED) );
  addMonGroup( new MonGroup(this, m_histdirrateratio, run, ATTRIB_UNMANAGED, "", "weightedEff") );
  addMonGroup( new MonGroup(this, m_histdircoverage, lowStat, ATTRIB_UNMANAGED) );
  //  addMonGroup( new MonGroup(this, m_histdireff,    run, ATTRIB_UNMANAGED, "", "weightedEff") );
  // YY 12.04.10 in order to avoid warning messages while merging by the post-processor 
  addMonGroup( new MonGroup(this, m_histdireff,    run, ATTRIB_UNMANAGED) );
  addMonGroup( new MonGroup(this, m_histdireffnumdenom,    run, ATTRIB_UNMANAGED) );
  addMonGroup( new MonGroup(this, m_histdirdist2d, run, ATTRIB_UNMANAGED) );

  //added by marx
  for(std::map<std::string, std::string>::iterator itmap=m_ztpmap.begin();itmap!=m_ztpmap.end();itmap++){ 
    m_histdirmuztp="HLT/MuonMon/MuZTP/" + itmap->second;
    addMonGroup( new MonGroup(this, m_histdirmuztp, run, ATTRIB_UNMANAGED) );
  }

  //setCurrentMonGroup("HLT/MuonMon/Efficiency");

  // define here all histograms
  //Common
  StatusCode scCommon;
  scCommon=bookCommonDQA();
  if( scCommon.isFailure() ){
    ATH_MSG_VERBOSE("bookCommonDQA failed");
  }

  //Chain
  StatusCode scChain;
  scChain=bookChainDQA();
  if( scChain.isFailure() ){
    ATH_MSG_VERBOSE("bookChainDQA failed");
  }

  //L2MuonSA
  StatusCode scL2MuonSA;
  scL2MuonSA=bookL2MuonSADQA();
  if( scL2MuonSA.isFailure() ){
    ATH_MSG_VERBOSE("bookL2MuonSADQA failed");
  }
  //muComb
  StatusCode scMuComb;
  scMuComb=bookMuCombDQA();
  if( scMuComb.isFailure() ){
    ATH_MSG_VERBOSE("bookMuCombDQA failed");
  }
  //muIso
  StatusCode scMuIso;
  scMuIso=bookMuIsoDQA();
  if( scMuIso.isFailure() ){
    ATH_MSG_VERBOSE("bookMuIsoDQA failed");
  }
  //TileMu
  StatusCode scTileMu;
  scTileMu=bookTileMuDQA();
  if( scTileMu.isFailure() ){
    ATH_MSG_VERBOSE("bookTileMuDQA failed");
  }
  //MuonEF
  StatusCode scMuonEF;
  scMuonEF=bookMuonEFDQA();
  if( scMuonEF.isFailure() ){
    ATH_MSG_VERBOSE("bookMuonEFDQA failed");
  }
  //MuGirl
  StatusCode scMuGirl;
  scMuGirl=bookMuGirlDQA();
  if( scMuGirl.isFailure() ){
    ATH_MSG_VERBOSE("bookMuGirlDQA failed");
  }
  //MuZTP
  StatusCode scMuZTP;
  scMuZTP=bookMuZTPDQA();
  if( scMuZTP.isFailure() ){
    ATH_MSG_VERBOSE("bookMuZTPDQA failed");
  }

  StatusCode sc = scCommon & scChain & scL2MuonSA & scMuComb & scMuIso & scTileMu & scMuonEF & scMuGirl & scMuZTP;
  return sc;
  // if(sc==1){
  //   return StatusCode::SUCCESS;
  // }else if(sc>1){
  //   return StatusCode::RECOVERABLE;
  // }

  // return StatusCode::FAILURE;
}

/*---------------------------------------------------------*/
StatusCode HLTMuonMonTool::fill()
  /*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG("Begin fill()");

  // fill offline muons
  StatusCode scRecMuon;
  scRecMuon=fillRecMuon();
  if( scRecMuon.isFailure() ){
    ATH_MSG_VERBOSE("fillRecMuon failed");
  }

  //commom tools
  StatusCode scCommon;
  scCommon=fillCommonDQA();
  if( scCommon.isFailure() ){
    ATH_MSG_VERBOSE("fillCommon failed");
  }
  hist("Common_Counter", m_histdir )->Fill((float)EVENT);
  if(!m_HI_pp_mode)hist("HI_PP_Flag", m_histdir)->Fill(0);
  /*
  auto chainGroup = getTDT()->getChainGroup("HLT_mu.*");
  for(auto &trig : chainGroup->getListOfTriggers()) {
    auto cg = getTDT()->getChainGroup(trig);
    std::string thisTrig = trig;
    ATH_MSG_DEBUG("muonhlt chainlist "<<thisTrig.c_str());
  }*/
  std::vector<std::string>::const_iterator it;
  int itr;
  std::vector<std::string> mchainlist;
  mchainlist.clear();
  for(it=m_chainsEFiso.begin(), itr=0; it != m_chainsEFiso.end() ; it++, itr++ ) mchainlist.push_back((*it).c_str());
  for(it=m_chainsMSonly.begin(), itr=0; it != m_chainsMSonly.end() ; it++, itr++ ) mchainlist.push_back((*it).c_str());
  for(it=m_chainsEFFS.begin(), itr=0; it != m_chainsEFFS.end() ; it++, itr++ ) mchainlist.push_back((*it).c_str());
  for(it=m_chainsGeneric.begin(), itr=0; it != m_chainsGeneric.end() ; it++, itr++ )mchainlist.push_back((*it).c_str());
  for(it=m_chainsLowpt.begin(), itr=0; it != m_chainsLowpt.end() ; it++, itr++ )mchainlist.push_back((*it).c_str());
  for(int i=0; i<(int)mchainlist.size(); i++){
    TString s=mchainlist[i];
    hist("Monitoring_Chain",m_histdir)->GetXaxis()->SetBinLabel(i+1,s);
  }
  for(int i=0; i<(int)mchainlist.size(); i++){
    if(getTDT()->isPassed(mchainlist[i]))hist("Monitoring_Chain",m_histdir)->Fill(i);
  }

  // chain
  StatusCode scChain;
  try {
    scChain=fillChainDQA();
    if( scChain.isFailure() ) {
      ATH_MSG_VERBOSE("fillChainDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by fillChainDQA");
    scChain=StatusCode::RECOVERABLE;
  }

  // L2MuonSA
  StatusCode scL2MuonSA;
  try {
    scL2MuonSA=fillL2MuonSADQA();
    if( scL2MuonSA.isFailure() ) {
      ATH_MSG_VERBOSE("fillL2MuonSADQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by fillL2MuonSADQA");
    scL2MuonSA=StatusCode::RECOVERABLE;
  }

  // muComb
  StatusCode scMuComb;
  try {
    scMuComb=fillMuCombDQA();
    if( scMuComb.isFailure() ) {
      ATH_MSG_VERBOSE("fillMuCombDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by fillMuCombDQA");
    scMuComb=StatusCode::RECOVERABLE;
  }

  // muIso
  StatusCode scMuIso;
  try {
    scMuIso=fillMuIsoDQA();
    if( scMuIso.isFailure() ) {
      ATH_MSG_VERBOSE("fillMuIsoDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by fillMuIsoDQA");
    scMuIso=StatusCode::RECOVERABLE;
  }

  // TileMu
  StatusCode scTileMu;
  try {
    scTileMu=fillTileMuDQA();
    if( scTileMu.isFailure() ) {
      ATH_MSG_VERBOSE("fillTileMuDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by fillTileMuDQA");
    scTileMu=StatusCode::RECOVERABLE;
  }

  // MuonEF
  StatusCode scMuonEF;
  try {
    scMuonEF=fillMuonEFDQA();
    if( scMuonEF.isFailure() ){
      ATH_MSG_VERBOSE("fillMuonEFDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by fillMuonEFDQA");
    scMuonEF=StatusCode::RECOVERABLE;
  }

  // MuGirl
  StatusCode scMuGirl;
  try {
    scMuGirl=fillMuGirlDQA();
    if( scMuGirl.isFailure() ){
      ATH_MSG_VERBOSE("fillMuGirlDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by fillMuGirlDQA");
    scMuGirl=StatusCode::RECOVERABLE;
  }

  // MuZTP
  StatusCode scMuZTP;
  try {
    scMuZTP=fillMuZTPDQA_wrapper();
    // scMuZTP=fillMuZTPDQA();
    if( scMuZTP.isFailure() ){
      ATH_MSG_VERBOSE("fillMuZTPDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by fillMuZTPDQA");
    scMuZTP=StatusCode::RECOVERABLE;
  }

  StatusCode sc = scCommon & scRecMuon & scChain & scL2MuonSA & scMuComb & scMuIso & scTileMu & scMuonEF & scMuGirl & scMuZTP;

  ATH_MSG_DEBUG( " scCommon " << scCommon  
		<< " scRecMuon " << scRecMuon 
		<< " scChain " << scChain
                << " scL2MuonSA " << scL2MuonSA
                << " scMuComb " << scMuComb
                << " scMuIso " << scMuIso
                << " scTileMu " << scTileMu
                << " scMuonEF " << scMuonEF
                << " scMuGirl " << scMuGirl
                << " scMuZTP " << scMuZTP
                << " sc " << sc);
  return sc;
  
  // if(sc==1){
  //   return StatusCode::SUCCESS;
  // }else if(sc>1){
  //   return StatusCode::RECOVERABLE;
  // }

  // return StatusCode::FAILURE;
}

/*---------------------------------------------------------*/
StatusCode HLTMuonMonTool::proc()
  /*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG("proc being called");

  //proc for common tools

  // chain analysis
  StatusCode scChain;
  try {
    scChain=procChainDQA();
    if( scChain.isFailure() ){
      ATH_MSG_VERBOSE("procChainDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by procChainDQA");
    scChain=StatusCode::RECOVERABLE;
  }

  // L2MuonSA
  StatusCode scL2MuonSA;
  try {
    scL2MuonSA=procL2MuonSADQA();
    if( scL2MuonSA.isFailure() ){
      ATH_MSG_VERBOSE("procL2MuonSADQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by procL2MuonSADQA");
    scL2MuonSA=StatusCode::RECOVERABLE;
  }

  // muComb
  StatusCode scMuComb;
  try {
    scMuComb=procMuCombDQA();
    if( scMuComb.isFailure() ){
      ATH_MSG_VERBOSE("procMuCombDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by procMuCombDQA");
    scMuComb=StatusCode::RECOVERABLE;
  }

  // muIso
  StatusCode scMuIso;
  try {
    scMuIso=procMuIsoDQA();
    if( scMuIso.isFailure() ){
      ATH_MSG_VERBOSE("procMuIsoDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by procMuIsoDQA");
    scMuIso=StatusCode::RECOVERABLE;
  }

  // TileMu
  StatusCode scTileMu;
  try {
    scTileMu=procTileMuDQA();
    if( scTileMu.isFailure() ){
      ATH_MSG_VERBOSE("procTileMuDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by procTileMuDQA");
    scTileMu=StatusCode::RECOVERABLE;
  }

  // MuonEF
  StatusCode scMuonEF;
  try {
    scMuonEF=procMuonEFDQA();
    if( scMuonEF.isFailure() ){
      ATH_MSG_VERBOSE("procMuonEFDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by procMuonEFDQA");
    scMuonEF=StatusCode::RECOVERABLE;
  }

  // MuGirl
  StatusCode scMuGirl;
  try {
    scMuGirl=procMuGirlDQA();
    if( scMuGirl.isFailure() ){
      ATH_MSG_VERBOSE("procMuGirlDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by procMuGirlDQA");
    scMuGirl=StatusCode::RECOVERABLE;
  }

  // MuZTP
  StatusCode scMuZTP;
  try {
    scMuZTP=procMuZTPDQA();
    if( scMuZTP.isFailure() ){
      ATH_MSG_VERBOSE("procMuZTPDQA failed");
    }
  }
  catch(...) {
    ATH_MSG_ERROR("Exception thrown by procMuZTPDQA");
    scMuZTP=StatusCode::RECOVERABLE;
  }

  //

  StatusCode sc = scChain & scL2MuonSA & scMuComb & scMuIso & scTileMu & scMuonEF & scMuGirl & scMuZTP;
  return sc;
  // if(sc==1){
  //   return StatusCode::SUCCESS;
  // }else if(sc>1){
  //   return StatusCode::RECOVERABLE;
  // }

  // return StatusCode::FAILURE;
}


const HLT::TriggerElement* HLTMuonMonTool :: getDirectSuccessorHypoTEForL2(const HLT::TriggerElement *te, std::string step, std::string chainname){

  //m_ExpertMethods->enable();
  std::string hyponame = "";
  if(step=="L2MuonSA") hyponame = m_hypomapL2SA[chainname];
  if(step=="L2muComb") hyponame = m_hypomapL2CB[chainname];
  const HLT::TriggerElement *hypote = NULL;
  std::vector<HLT::TriggerElement*> TEsuccessors = m_ExpertMethods->getNavigation()->getDirectSuccessors(te);
  for(auto te2 : TEsuccessors){
    ATH_MSG_VERBOSE("[" << chainname <<"] ::TE2: " << te2->getId() << " " <<  Trig::getTEName(*te2) );
    if(Trig::getTEName(*te2)==hyponame){
      ATH_MSG_DEBUG("[" << chainname<< "] selected HypoTE: " << te2->getId() << " " <<  Trig::getTEName(*te2) <<  " isPassed=" << te2->getActiveState() );
      hypote = te2;
    }
  }
  if(!hypote){
      ATH_MSG_DEBUG("[" << chainname<< "] HypoTE not found. Assigning alg TE.");
      hypote = te;
  }

  return hypote;
}
