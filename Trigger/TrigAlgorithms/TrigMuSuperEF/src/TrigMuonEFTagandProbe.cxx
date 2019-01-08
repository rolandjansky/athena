/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration 
*/

/*--------------------------------------------------------------------------------------
  Event Filter Z_mu+mu- Online Tag and Probe algorithm
  Authors: Jonathan Jamieson, University of Glasgow, Created: 09/10/2017, Last edit: 31/05/2018
  
  Summary:-----------------------------------------------------------------------------------------  
  
  Accesses collection of xaod::muons within event and picks out only 'good muons', i.e. muons formed from combined (NOT Stand Alone) tracks including any duplicates and with isolation criteria DeltaR > 0.2 
  
  Events passed from preceding Mu22_mu8noL1 HLT trigger are trimmed by first applying a 10 GeV cut on pt to remove events with pt below that expected in z_mu+mu- events (10 GeV picked to not be too high as to bias against low momentum Z events and to stay consistent with previous work
  
  >2 muon events are then trimmed to exact pairs by matching all the avaliable opposite charge muons and picking the best fit to the Z mass, applying a default +-10 GeV threshold cut around the Z mass to match the eventual pair requirement. 
  
  LVL 1 trigger information is then accessed for the event and Delta R values are calculated for each RoI and muon pair 
  
  Tag and probe selection: 
  A tag muon is defined as any good muon with pt > 10GeV that can be associated with a LVL 1 RoI (DeltaR <0.1) belonging to LVL1 threshold >= threshold 5 (MU20))
  A candidate probe muon is any other muon with pt > 10GeV with the additional requirement on tag and probe pairs that their invariant mass be within +-10GeV from the Z mass 

  Efficiency plots for three kinematic variables (eta,phi and pT) are avaliable, produced from TProfile objects created from two vectors
  -kinematic values for every probe candidate [e.g: EF_Phi_Total_thr1_b] 
  -1.0 or 0.0 for every probe candidate denoting RoI match (1) or no matching L1 info (0)

  For probe muon candidates without a matching L1 RoI: count 0 for match in all thresholds to avoid biasing for higher efficiencies at pTs lower than threshold acceptance.

  Seperate plots for each threshold as well as barrel and endcap where appropriate are produced for all kinemetics and a flat PT cut of 25GeV applied to data in Eta and Phi plots for all thresholds to avoid poor efficiency at to low pT turn on. 

  Eta barrel and endcap statistics are merged to facilitate more sensible plotting of efficiencies but seperated statistics still required for producing 2d plots vs Phi

  ([31/05/2018] pT plots, split Eta plots and Threshold 2-4 2D plots have been removed from python plotting code: "TrigMuonEFTagandProbeMonitoring.py" to reduce memory allocation, monitored containers for these are still filled)

  Running:-----------------------------------------------------------------------------------------  

  athenaHLT: 
  from glob import glob;athenaHLT.py -M -f "glob("/path/to/dir1/star")+glob("/path/to/dir2/star")+glob(etc..)" -o HLT_MCV7  -c 'testMCV7=True;HLTOutputLevel=INFO;ServiceMgr.MessageSvc.defaultLimit=9999999;doValidation=True;fpeAuditor=True' -C "topSequence.TrigSteer_HLT.TrigMuonEFTagandProbe.OutputLevel=DEBUG" TriggerRelease/runHLT_standalone.py >& log 

  Output of online histogramming will be in r000...HLT-Histogramming.root

  athena (data):

  athena.py -c 'testMCV7=True;HLTOutputLevel=INFO;ServiceMgr.MessageSvc.defaultLimit=9999999;doValidation=True;fpeAuditor=True;from glob import glob;BSRDOInput=glob("/path/to/dir1/star")+glob("/path/to/dir2/star")+glob(etc..)'  TriggerRelease/runHLT_standalone.py Path/To/localDebug.py >& log_file

  athena (MC):

/usr/bin/time -f "mem=%K RSS=%M elapsed=%E cpu.sys=%S .user=%U" athena.py -c 'menu="MC_pp_v7";sliceName="muon";ServiceMgr.MessageSvc.defaultLimit=9999999;EvtMax=-1;from glob import glob;jp.AthenaCommonFlags.FilesInput=glob("/path/to/dir1/star")+glob("/path/to/dir2/star")+glob(etc..)' TriggerTest/testCommonSliceAthenaTrigRDO.py Path/To/localDebug.py >& log_file_MC
*/


#include <iomanip>
#include <cmath>
#include "TrigMuonEFTagandProbe.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "FourMomUtils/xAODP4Helpers.h"

#include "GaudiKernel/SystemOfUnits.h"

//Monitoring object constructor
MonitoringObject::MonitoringObject() {
  
  //Sets up vector of containers for each threshold so filling functions can pick correct threshold by element index
  m_ThreshMon.push_back(m_Threshold1);
  m_ThreshMon.push_back(m_Threshold2);
  m_ThreshMon.push_back(m_Threshold3);
  m_ThreshMon.push_back(m_Threshold4);
  m_ThreshMon.push_back(m_Threshold5);
  m_ThreshMon.push_back(m_Threshold6);

}


//Constructor for new algorithm class inheriting from base FexAlgo class
//ISvcLocator is service locator for shared global services between algorithms (e.g. magnetic field information etc)
TrigMuonEFTagandProbe::TrigMuonEFTagandProbe(const std::string &name, ISvcLocator *pSvcLocator) : 
  FexAlgo(name, pSvcLocator),
  m_debug(false),
  m_verbose(false)

{

  declareMonitoredStdContainer("EF_Eta_Total_thr1_b",       m_eta_total_thr1_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr1_b",       m_phi_total_thr1_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr1_b",        m_pt_total_thr1_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr1_e",       m_eta_total_thr1_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr1_e",       m_phi_total_thr1_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr1_e",        m_pt_total_thr1_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr1",         m_eta_total_thr1,            IMonitoredAlgo::AutoClear);


  declareMonitoredStdContainer("EF_Eta_Total_thr2_b",       m_eta_total_thr2_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr2_b",       m_phi_total_thr2_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr2_b",        m_pt_total_thr2_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr2_e",       m_eta_total_thr2_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr2_e",       m_phi_total_thr2_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr2_e",        m_pt_total_thr2_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr2",         m_eta_total_thr2,            IMonitoredAlgo::AutoClear);


  declareMonitoredStdContainer("EF_Eta_Total_thr3_b",       m_eta_total_thr3_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr3_b",       m_phi_total_thr3_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr3_b",        m_pt_total_thr3_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr3_e",       m_eta_total_thr3_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr3_e",       m_phi_total_thr3_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr3_e",        m_pt_total_thr3_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr3",         m_eta_total_thr3,            IMonitoredAlgo::AutoClear);


  declareMonitoredStdContainer("EF_Eta_Total_thr4_b",       m_eta_total_thr4_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr4_b",       m_phi_total_thr4_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr4_b",        m_pt_total_thr4_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr4_e",       m_eta_total_thr4_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr4_e",       m_phi_total_thr4_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr4_e",        m_pt_total_thr4_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr4",         m_eta_total_thr4,            IMonitoredAlgo::AutoClear);


  declareMonitoredStdContainer("EF_Eta_Total_thr5_b",       m_eta_total_thr5_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr5_b",       m_phi_total_thr5_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr5_b",        m_pt_total_thr5_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr5_e",       m_eta_total_thr5_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr5_e",       m_phi_total_thr5_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr5_e",        m_pt_total_thr5_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr5",         m_eta_total_thr5,            IMonitoredAlgo::AutoClear);


  declareMonitoredStdContainer("EF_Eta_Total_thr6_b",       m_eta_total_thr6_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr6_b",       m_phi_total_thr6_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr6_b",        m_pt_total_thr6_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr6_e",       m_eta_total_thr6_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr6_e",       m_phi_total_thr6_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr6_e",        m_pt_total_thr6_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Total_thr6",         m_eta_total_thr6,            IMonitoredAlgo::AutoClear);



  //Need three seperate eff vectors as Totals are different sizes for Eta unsplit, phi&eta split and pT split
  declareMonitoredStdContainer("EF_Eta_Eff_thr1",           m_eta_eff_thr1,              IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Eff_thr2",           m_eta_eff_thr2,              IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Eff_thr3",           m_eta_eff_thr3,              IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Eff_thr4",           m_eta_eff_thr4,              IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Eff_thr5",           m_eta_eff_thr5,              IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Eff_thr6",           m_eta_eff_thr6,              IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Etaphi_Eff_thr1_b",      m_etaphi_eff_thr1_b,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr1_e",      m_etaphi_eff_thr1_e,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr2_b",      m_etaphi_eff_thr2_b,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr2_e",      m_etaphi_eff_thr2_e,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr3_b",      m_etaphi_eff_thr3_b,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr3_e",      m_etaphi_eff_thr3_e,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr4_b",      m_etaphi_eff_thr4_b,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr4_e",      m_etaphi_eff_thr4_e,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr5_b",      m_etaphi_eff_thr5_b,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr5_e",      m_etaphi_eff_thr5_e,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr6_b",      m_etaphi_eff_thr6_b,         IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Etaphi_Eff_thr6_e",      m_etaphi_eff_thr6_e,         IMonitoredAlgo::AutoClear);

  
  declareMonitoredStdContainer("EF_Pt_Eff_thr1_b",          m_pt_eff_thr1_b,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr1_e",          m_pt_eff_thr1_e,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr2_b",          m_pt_eff_thr2_b,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr2_e",          m_pt_eff_thr2_e,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr3_b",          m_pt_eff_thr3_b,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr3_e",          m_pt_eff_thr3_e,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr4_b",          m_pt_eff_thr4_b,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr4_e",          m_pt_eff_thr4_e,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr5_b",          m_pt_eff_thr5_b,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr5_e",          m_pt_eff_thr5_e,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr6_b",          m_pt_eff_thr6_b,             IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Eff_thr6_e",          m_pt_eff_thr6_e,             IMonitoredAlgo::AutoClear);

}

TrigMuonEFTagandProbe::~TrigMuonEFTagandProbe(){} //Destructor


HLT::ErrorCode TrigMuonEFTagandProbe::hltInitialize()
{

  // Conditional debug flags for when debug output requires extra compute
  m_debug = msgLvl() <= MSG::DEBUG; // MSG levels are 1-7, VERBOSE,DEBUG,INFO,WARNINGS,ERROR,FATAL,ALWAYS
  m_verbose = msgLvl() <= MSG::VERBOSE;

  ATH_MSG_DEBUG("TrigMuonEFTagandProbe::hltInitialize()"); 
  ATH_MSG_DEBUG("TrigMuonEFTagandProbe::hltInitialize(); building framework for monitored variables"); 

  //Set up Monitoring framework to handle sensible filling of per threshold monitoring values for efficiency calculations

  //Build seperate containers for each monitored variable within each threshold
  //------------------------ORDER OF VARIABLES MUST BE RETAINED---------------------------
  //Order for filling is Barrel/Endcap/merged THEN eta/phi/etaphi/pt THEN total/efficiency
  //Threshold number must be unsigned int between 1 and 6 inclusive

  m_Thresh_Mon.build_threshold(1, //Threshold Number 
			     m_eta_total_thr1_b, //Eta values for all probe candidates passing threshold 1 within the barrel with extra 25GeV pT cut applied
			     m_phi_total_thr1_b, //Phi values """ with extra 25GeV pT cut applied
			     m_etaphi_eff_thr1_b, // 1.0 for all matched probes passing threshold 1 in barrel with extra 25GeV pT cut, 0.0 otherwise
			     m_pt_total_thr1_b, //Pt values for all probe candidates excluding 25GeV cut
			     m_pt_eff_thr1_b, //1.0 """ excluding 25GeV cut, 0.0 otherwise
			     m_eta_total_thr1_e, //Eta values """ within the endcaps, pT cut
			     m_phi_total_thr1_e, //Phi, endcap, pT cut
			     m_etaphi_eff_thr1_e, // 1.0 """ in endcap with 25GeV cut
			     m_pt_total_thr1_e, //pT values """ within endcap, excluding pT cut
			     m_pt_eff_thr1_e, //1.0 """ excluding 25GeV cut, endcap
			     m_eta_total_thr1, //Eta values for all probe candidates passing thresh 1 in full detector volume, including 25GeV pT cut
			     m_eta_eff_thr1); //1.0 """ full detector volume, including pT cut
  
  m_Thresh_Mon.build_threshold(2,
			     m_eta_total_thr2_b, 
			     m_phi_total_thr2_b, 
			     m_etaphi_eff_thr2_b, 
			     m_pt_total_thr2_b,
			     m_pt_eff_thr2_b, 
			     m_eta_total_thr2_e,
			     m_phi_total_thr2_e,
			     m_etaphi_eff_thr2_e,
			     m_pt_total_thr2_e,
			     m_pt_eff_thr2_e,
			     m_eta_total_thr2,
			     m_eta_eff_thr2);

  m_Thresh_Mon.build_threshold(3,
			     m_eta_total_thr3_b, 
			     m_phi_total_thr3_b, 
			     m_etaphi_eff_thr3_b, 
			     m_pt_total_thr3_b,
			     m_pt_eff_thr3_b, 
			     m_eta_total_thr3_e,
			     m_phi_total_thr3_e,
			     m_etaphi_eff_thr3_e,
			     m_pt_total_thr3_e,
			     m_pt_eff_thr3_e,
			     m_eta_total_thr3,
			     m_eta_eff_thr3);

  m_Thresh_Mon.build_threshold(4,
			     m_eta_total_thr4_b, 
			     m_phi_total_thr4_b, 
			     m_etaphi_eff_thr4_b, 
			     m_pt_total_thr4_b,
			     m_pt_eff_thr4_b, 
			     m_eta_total_thr4_e,
			     m_phi_total_thr4_e,
			     m_etaphi_eff_thr4_e,
			     m_pt_total_thr4_e,
			     m_pt_eff_thr4_e,
			     m_eta_total_thr4,
			     m_eta_eff_thr4);

  m_Thresh_Mon.build_threshold(5,
			     m_eta_total_thr5_b, 
			     m_phi_total_thr5_b, 
			     m_etaphi_eff_thr5_b, 
			     m_pt_total_thr5_b,
			     m_pt_eff_thr5_b, 
			     m_eta_total_thr5_e,
			     m_phi_total_thr5_e,
			     m_etaphi_eff_thr5_e,
			     m_pt_total_thr5_e,
			     m_pt_eff_thr5_e,
			     m_eta_total_thr5,
			     m_eta_eff_thr5);
  
  m_Thresh_Mon.build_threshold(6,
			     m_eta_total_thr6_b, 
			     m_phi_total_thr6_b, 
			     m_etaphi_eff_thr6_b, 
			     m_pt_total_thr6_b,
			     m_pt_eff_thr6_b, 
			     m_eta_total_thr6_e,
			     m_phi_total_thr6_e,
			     m_etaphi_eff_thr6_e,
			     m_pt_total_thr6_e,
			     m_pt_eff_thr6_e,
			     m_eta_total_thr6,
			     m_eta_eff_thr6);

  ATH_MSG_INFO("TrigMuonEFTagandProbe Initialization completed successfully");
 
  return HLT::OK;
}



HLT::ErrorCode TrigMuonEFTagandProbe::hltFinalize()
{
  ATH_MSG_DEBUG("TrigMuonEFTagandProbe::hltFinalize()"); 

  return HLT::OK;
}


HLT::ErrorCode TrigMuonEFTagandProbe::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout) 
{

  //Reset temporary containers
  m_good_muons.clear(); //holds all combined muons

  ATH_MSG_DEBUG("TrigMuonEFTagandProbe::hltExecute()"); 


  //DEBUG to check if Trigger events are being passed to algorithm
  if (m_debug){
    std::string teInLabel;
    std::string teOutLabel;
    TrigConf::HLTTriggerElement::getLabel( inputTE->getId(), teInLabel );
    TrigConf::HLTTriggerElement::getLabel( TEout->getId(), teOutLabel );
    
    ATH_MSG_DEBUG("TrigMuonEFTagandProbeExecute() chain Trigger Event input ID=" << inputTE->getId() << " Trigger Event output ID=" << TEout->getId() << " Input Label=" << teInLabel << " Output Label=" << teOutLabel);
  }


  //-----------------Access LVL1 trigger ROI information--------------------------------------------------

  
  //Have to access from initialNode as AllTE combiner algorithms earlier in chain remove regular access back to seeding RoIs through getFeature
  //Check for LVL1 muon RoI information in current event first, if there is no info then we shouldn't bother calculating anything else and should move to next event 
  
  std::vector<const LVL1::RecMuonRoI*> l1_muon_RoIs;

  HLT::Navigation* nav = config()->getNavigation();
  HLT::TriggerElement* initialTE = nav->getInitialNode(); //Get initial TE that chain is seeded from
  const std::vector<HLT::TriggerElement*>& inputTE_RoIs_nav = nav->getDirectSuccessors(initialTE); //Get all first generation TEs seeded from initialTE

  for (auto TERoI : inputTE_RoIs_nav) { //Loop through TEs (RoIs)
    const LVL1::RecMuonRoI* RoI;
    if(getFeature(TERoI, RoI)!=HLT::OK) { //Check TE for L1 RoI information
      ATH_MSG_DEBUG("No LVL1::RecMuonRoI Feature found in initial RoI Node"); //If getfeature fails
      return HLT::MISSING_FEATURE;
    }
    if (!RoI) { // if L1 Muon RoI entry is null
      ATH_MSG_DEBUG("Empty LVL1::RecMuonRoI feature found, trying another Trigger Element");
    }
    else{
      l1_muon_RoIs.push_back(RoI);
    }
  }

  if(l1_muon_RoIs.empty()){ //Save compute by killing if there is no L1 information
    ATH_MSG_DEBUG("L1 RoI size = 0, moving to next event");
    return HLT::OK; 
  }
  
  ATH_MSG_DEBUG("Number of L1 RoIs found = "<<l1_muon_RoIs.size());    



  //---------------------Get muonContainer linked to inputTE---------------------------------------------

  const xAOD::MuonContainer*  muonContainer=0; 

  // getFeature takes a trigger element (inputTE) and a target feature (MuonContainer) and fills the target with the feature found in the trigger element, Output is a HLT::code, but OK does NOT mean a matching, non zero feature was found just that the feature exists within the trigger element
 
  if(getFeature(inputTE, muonContainer)!=HLT::OK) {  //If getfeature fails
    ATH_MSG_DEBUG("no xAOD::MuonContainer Feature found");
    return HLT::MISSING_FEATURE;
  } 



  else { //If get feature succeeds
    if (!muonContainer) { // if muonContainer entry is null
      ATH_MSG_DEBUG("null xAOD::MuonContainer Feature found");
      return HLT::MISSING_FEATURE;
    } 
    
    ATH_MSG_DEBUG("xAOD::MuonContainer found with size " << muonContainer->size());
  }
  

  // Initialise temporary value containers
  float deltaR=0.0; 
  unsigned int tag_thresh=0;
  unsigned int probe_thresh=0;

  struct TaPMuon tagmuon, probemuon; //TaP Muon structures to group muon and RoI values into one object
  

  //----------------------Loop over EF muons within event ------------------------------------------------


  for(auto muon : *muonContainer) {
                              
    if (!muon) { // Check if muon object is null
      ATH_MSG_DEBUG("No xAOD::Muon found.");
      continue; //move to next muon
    } 
    else {

      if (muon->primaryTrackParticle()) { //was there a muon in this RoI? (check muon hasn't been stripped of information)

	//Checking muon is good (i.e. EF Combined muon NOT EF stand alone muon)
        const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::CombinedTrackParticle); 


	//------------------------10 GeV minimum pT cut applied--------------------------------------------

        if (!tr || tr->pt()/Gaudi::Units::GeV < 10.0) { // if this muon has NOT been formed from a combined track > 10GeV

          ATH_MSG_DEBUG("Bad muon is of type: " << muon->muonType() << " With 0=combined, 1=MuonSA, 2=SegmentTagged, 3=CaloTagged, 4=SiliconAssociatedForwardMuon, Abs pt = " << muon->pt()/Gaudi::Units::GeV << " GeV. Charge=" << muon->charge() << " Psuedorapidity = " << muon->eta() << " Phi = " << muon->phi());
          continue;
        } 

	//---------------------------Collect good muons--------------------------------------------------

	else { //Current muon is formed from combined track
	  ATH_MSG_DEBUG("Retrieved muon of type " << muon->muonType() << ": Combined track with abs pt " << tr->pt()/Gaudi::Units::GeV << " GeV. Charge=" << tr->charge() << " match chi2 = " << tr->chiSquared() << " Psuedorapidity = " << tr->eta() << " Phi = " << tr->phi());
	  
	  m_good_muons.push_back(muon);
	}
      }
    }
  }
  

  //------------------------Check and ensure exactly 2 good muons per event--------------------------------

  if (m_good_muons.size()<2){
    ATH_MSG_WARNING("Less than 2 combined muons before trimming so event will be ignored, moving to next event");
    return HLT::OK;
  }


  if (m_good_muons.size()>2 || (m_good_muons)[0]->charge() == (m_good_muons)[1]->charge())
    {
      ATH_MSG_DEBUG(">2 muons found or matching charge found, no. of \"good\" muons = " << m_good_muons.size());
      trim_container(m_good_muons); //Performs trimming of muon container reducing to size = 2
      
      if (m_good_muons.size()!=2){
	ATH_MSG_DEBUG("More/Less than 2 good muons after trimming so further event analysis will be ignored, moving to next event"); //Extra safety check although trim_container should only allow exit with exactly 2 muons
	return HLT::OK;
      }
      
      ATH_MSG_DEBUG("container trimmed, no. of good muons = " << m_good_muons.size());
    }

  //---------------------------Check isolation criteria for muon pair---------------------------------------
  //Require deltaR > 0.2 between the two muons in order to keep probe muons unbiased from the event trigger

  float TaPdeltaR = (m_good_muons)[0]->p4().DeltaR((m_good_muons)[1]->p4());
  ATH_MSG_DEBUG("TaP DeltaR value = " << TaPdeltaR);

  if (TaPdeltaR <= 0.2){
    ATH_MSG_WARNING("Tag and Probe not sufficiently isolated for unbiased efficiency; DeltaR = "<< TaPdeltaR <<" <= 0.2, moving to next event, LocalReadTest");
    return HLT::OK;
  }


  //-------------------Calculate dimuon invariant mass from 4 momenta for probe selection----------------
    
  float invMass_dimuon = (((m_good_muons)[0]->p4() + (m_good_muons)[1]->p4()).M())/Gaudi::Units::GeV;
  
  ATH_MSG_DEBUG("final size of good muon container = " << m_good_muons.size());
  ATH_MSG_DEBUG("Dimuon Invariant Mass from 4mom  = " << invMass_dimuon);



  //--------------------Perform Tag L1 matching to produce Tag and Probe pairs--------------------------

  //Define a vector of pairs containing TaPmuon structure object and Probe muon i.e. tag and probe pairs
  std::vector<std::pair <TaPMuon,const xAOD::Muon*> > TaP;

  //A check so we don't bother trying to match muons to same RoI as isolation criteria already applied
  const LVL1::RecMuonRoI* sharedRoItest=0; 

  for (unsigned int i=0;i<m_good_muons.size();i++){ //loop over good muons

    //For each muon calculate all Delta R values in scope
    for (auto roi : l1_muon_RoIs){
      
      //xAOD::P4Helpers class required for DeltaR calculation as RoI objects don't have 4 momenta
      deltaR = xAOD::P4Helpers::deltaR((*(m_good_muons)[i]), roi->eta(), roi->phi(), false); //UseRapidity=false
      
      ATH_MSG_DEBUG("DeltaR value = " << deltaR); 
      tag_thresh=roi->getThresholdNumber();
      ATH_MSG_DEBUG("Threshold_ALL value = " << tag_thresh); 
      

      //---------------------------------Apply tag muon criteria---------------------------------------

      if (abs(91.1876-invMass_dimuon)<10.0 && deltaR<0.1 && roi!=sharedRoItest && tag_thresh>=5) {
	sharedRoItest=roi; //Potentially removes extra loops as isolation criteria doesn't allow shared RoIs anyway

	ATH_MSG_DEBUG("Threshold_Tag value = " << tag_thresh);

	tagmuon.Mu = m_good_muons[i]; //Tag muon
	tagmuon.DR = deltaR; //Tag muon DeltaR
	tagmuon.Thresh = tag_thresh; // Tag muon matched L1 threshold number
	tagmuon.RoI = roi; //Tag muon match L1 RoI

	//construct Tag and Probe pair including extra tag muon information
	TaP.push_back(std::make_pair(tagmuon,m_good_muons[1-i])); //1-i index is safe as events are checked to be size=2
	break; //don't need to check other RoIs once a match is found
      }
    }
  }

  ATH_MSG_DEBUG("NUMBER of tag and probe pairs = " << TaP.size()); 


  
  //-----------------Perform Probe L1 matching to fill Total and Passing histograms for each threshold--------------------

  for (unsigned int i=0;i<TaP.size();i++){ //Loop over each tag and Probe pair

    //if Probe is also a tag then Tag matching isn't required, go straight to filling kinemetics
    if (TaP[i].second==(TaP[1-i].first).Mu){ 
      
      //Fill monitoring variables for different probe thresholds inclusively (probe muon info,Threshold MonitoringObject)
      match_thresh(TaP[1-i].first,m_Thresh_Mon);
      continue;
    }

    //If probe isn't a tag then loop over RoIs and look for a L1 match that is different from the Tag L1
    for (unsigned int j=0;j<l1_muon_RoIs.size();j++){

      //xAOD::P4Helpers class required for DeltaR calculation as RoI objects don't have 4 momenta
      deltaR = xAOD::P4Helpers::deltaR((*(TaP[i].second)), l1_muon_RoIs[j]->eta(), l1_muon_RoIs[j]->phi(), false);

      if (deltaR<0.1 && l1_muon_RoIs[j]!=(TaP[1-i].first).RoI){ //If probe passes LVL1 match criteria

	probe_thresh=l1_muon_RoIs[j]->getThresholdNumber();

	ATH_MSG_DEBUG("Threshold_Probe value = " << probe_thresh);

	probemuon.Mu = TaP[i].second; //Probe Muon
        probemuon.DR = deltaR; //Probe muon DeltaR
        probemuon.Thresh = probe_thresh; //Probe muon matched L1 threshold number

	match_thresh(probemuon,m_Thresh_Mon); //Fill monitoring variables (probe muon info, Threshold MonitoringObject)
	break;
      } 

      //If Probe muon cannot be L1 matched
      if (j==(l1_muon_RoIs.size()-1)){

	match_thresh(TaP[i].second,m_Thresh_Mon); //Fill monitoring variables for unmatched probe (probe muon, MonitoringObject)
	
      }
    }
  }

  return HLT::OK;
  
}


//-----------------------------------Monitoring variable filling functions------------------------------------

//-------------------------------------If Probe is successfully matched---------------------------------------
//-------------------------------------Fill 1.0 into Efficiency histograms------------------------------------
void TrigMuonEFTagandProbe::match_thresh (TaPMuon tap ,MonitoringObject Thresh_Mon){

  //Barrel
  if (abs(tap.Mu->eta())<1.05){
    for (unsigned int i=0;i<tap.Thresh;i++){ //Loop over all thresholds <= current probe threshold value
      if ((tap.Mu->pt()/Gaudi::Units::GeV) > 25.0){ //Apply extra 25GeV pT cut to eta and phi variables

	//Fills eta values within barrel for all probe candidates matching threshold i+1 
	//i must be unsigned int between 0 and 5 inclusive
	Thresh_Mon.fill_eta_total_barrel(i,tap.Mu->eta()); //(Threshold Number,kinematic)
	Thresh_Mon.fill_phi_total_barrel(i,tap.Mu->phi());
	Thresh_Mon.fill_etaphi_matchfail_barrel(i,1.0); //(Threshold Number,Pass(1.0)/Fail(0.0))
	Thresh_Mon.fill_eta_total_merged(i,tap.Mu->eta());
	Thresh_Mon.fill_eta_matchfail_merged(i,1.0);
      }

      Thresh_Mon.fill_pt_total_barrel(i,tap.Mu->pt()/Gaudi::Units::GeV);
      Thresh_Mon.fill_pt_matchfail_barrel(i,1.0);

    }
  }

  //Endcap
  else {
    for (unsigned int i=0;i<tap.Thresh;i++){
      if ((tap.Mu->pt()/Gaudi::Units::GeV) > 25.0){

	Thresh_Mon.fill_eta_total_endcap(i,tap.Mu->eta());
	Thresh_Mon.fill_phi_total_endcap(i,tap.Mu->phi());
	Thresh_Mon.fill_etaphi_matchfail_endcap(i,1.0);
	Thresh_Mon.fill_eta_total_merged(i,tap.Mu->eta());
	Thresh_Mon.fill_eta_matchfail_merged(i,1.0);
      }

      Thresh_Mon.fill_pt_total_endcap(i,tap.Mu->pt()/Gaudi::Units::GeV);
      Thresh_Mon.fill_pt_matchfail_endcap(i,1.0);

    }
  }
  
}


//-------------------------------------If Probe cannot be successfully matched---------------------------
//-------------------------------------Fill 0.0 into Efficiency histograms---------------------------------
void TrigMuonEFTagandProbe::match_thresh (const xAOD::Muon* muon , MonitoringObject Thresh_Mon){

  //Barrel
  if (abs(muon->eta())<1.05){
    for (unsigned int i=0;i<6;i++){
      if ((muon->pt()/Gaudi::Units::GeV) > 25.0){

	Thresh_Mon.fill_eta_total_barrel(i,muon->eta());
	Thresh_Mon.fill_phi_total_barrel(i,muon->phi());
	Thresh_Mon.fill_etaphi_matchfail_barrel(i,0.0); 
	Thresh_Mon.fill_eta_total_merged(i,muon->eta());
	Thresh_Mon.fill_eta_matchfail_merged(i,0.0);
      }

      Thresh_Mon.fill_pt_total_barrel(i,muon->pt()/Gaudi::Units::GeV);
      Thresh_Mon.fill_pt_matchfail_barrel(i,0.0);

    }
  }

  //Endcap
  else {
    for (unsigned int i=0;i<6;i++){
      if ((muon->pt()/Gaudi::Units::GeV) > 25.0){


	Thresh_Mon.fill_eta_total_endcap(i,muon->eta());
	Thresh_Mon.fill_phi_total_endcap(i,muon->phi());
	Thresh_Mon.fill_etaphi_matchfail_endcap(i,0.0);
	Thresh_Mon.fill_eta_total_merged(i,muon->eta());
	Thresh_Mon.fill_eta_matchfail_merged(i,0.0);
      }

      Thresh_Mon.fill_pt_total_endcap(i,muon->pt()/Gaudi::Units::GeV);
      Thresh_Mon.fill_pt_matchfail_endcap(i,0.0);
    }
  }
}


//--------------------------------------Event trimming function--------------------------------------

//Function to resolve cases of >2 muons or 2 muons with same sign within muon container
//Loops over muon pairs, check for opposite sign and evaluates diiference between Z mass and pair invariant mass
//Picks pair with smallest mass difference < 10.0GeV
void TrigMuonEFTagandProbe::trim_container (std::vector<const xAOD::Muon*>& good_muons){

  std::vector<const xAOD::Muon*> best_muon_pair; //Holds current best pair

  double mZdiff = 10.0; // Default cut to match condition for valid tag and probe pair
  for (unsigned int itmu1=0;itmu1<good_muons.size();itmu1++){ //Loop over muons
    for (unsigned int itmu2=itmu1+1;itmu2<good_muons.size();itmu2++){ //Loop over remaining muons

      const xAOD::Muon *mu1 = good_muons[itmu1]; 
      const xAOD::Muon *mu2 = good_muons[itmu2];

      if( (mu1->charge() * mu2->charge()) > 0 ){ //Look for same charge muons
	ATH_MSG_DEBUG("(trim_container) only found muons of one sign, moving to next pair");
	continue;
      }

      double tempinvmass = abs(91.1876-((mu1->p4() + mu2->p4()).M())/Gaudi::Units::GeV);
      if (tempinvmass < mZdiff){ //Found a new best pair
	best_muon_pair.resize(2, nullptr);
	best_muon_pair[0] = mu1;
	best_muon_pair[1] = mu2;
	mZdiff = tempinvmass; //Set new best mass difference
      } 
    }
  }
  
  if( best_muon_pair.empty() ){    
    ATH_MSG_DEBUG("(trim_container) Found no viable muon pairs, clearing container and moving on");
    good_muons.clear();
    return;
  }

  ATH_MSG_DEBUG("(trim_container) best match found, current number of muons in pair = "<<best_muon_pair.size());
  
  good_muons.clear();
  good_muons = best_muon_pair;

  ATH_MSG_DEBUG("(trim_container) best match found, current size of good_muons = "<<good_muons.size());
}


//-------------------------------------Threshold Monitoring functions------------------------------------
//-------------------------------------Public functions within class MonitoringObject--------------------

//Simple function to build ordered container for variables in each threshold
//------------------ORDER OF VARIABLES IN EACH THRESHOLD CONTAINER IS IMPORTANT------------------------
void MonitoringObject::build_threshold (unsigned int thresh, 
		      std::vector<float> &eta_total_b,
		      std::vector<float> &phi_total_b,
		      std::vector<float> &etaphi_eff_b,
		      std::vector<float> &pt_total_b,
		      std::vector<float> &pt_eff_b,
		      std::vector<float> &eta_total_e,
		      std::vector<float> &phi_total_e,
		      std::vector<float> &etaphi_eff_e,
		      std::vector<float> &pt_total_e,
		      std::vector<float> &pt_eff_e,
		      std::vector<float> &eta_total_merge,
		      std::vector<float> &eta_eff_merge){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive

  else {
    m_ThreshMon[thresh-1].push_back(&eta_total_b);
    m_ThreshMon[thresh-1].push_back(&phi_total_b);
    m_ThreshMon[thresh-1].push_back(&etaphi_eff_b);
    m_ThreshMon[thresh-1].push_back(&pt_total_b);
    m_ThreshMon[thresh-1].push_back(&pt_eff_b);
    m_ThreshMon[thresh-1].push_back(&eta_total_e);
    m_ThreshMon[thresh-1].push_back(&phi_total_e);
    m_ThreshMon[thresh-1].push_back(&etaphi_eff_e);
    m_ThreshMon[thresh-1].push_back(&pt_total_e);
    m_ThreshMon[thresh-1].push_back(&pt_eff_e);
    m_ThreshMon[thresh-1].push_back(&eta_total_merge);
    m_ThreshMon[thresh-1].push_back(&eta_eff_merge);
  }
}


//Simple filling functions for each variable type and each threshold value 
//--------------------ORDER OF VARIABLES IN THRESHOLD CONTAINERS IS IMPORTANT------------------------

void MonitoringObject::fill_eta_total_barrel (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][0]->push_back(value);
} 


void MonitoringObject::fill_phi_total_barrel (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][1]->push_back(value);
} 


void MonitoringObject::fill_etaphi_matchfail_barrel (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][2]->push_back(value);
}


void MonitoringObject::fill_pt_total_barrel (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][3]->push_back(value);
}


void MonitoringObject::fill_pt_matchfail_barrel (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][4]->push_back(value);
}


void MonitoringObject::fill_eta_total_endcap (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][5]->push_back(value);
} 


void MonitoringObject::fill_phi_total_endcap (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][6]->push_back(value);
} 


void MonitoringObject::fill_etaphi_matchfail_endcap (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][7]->push_back(value);
}


void MonitoringObject::fill_pt_total_endcap (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][8]->push_back(value);
}


void MonitoringObject::fill_pt_matchfail_endcap (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][9]->push_back(value);
}


void MonitoringObject::fill_eta_total_merged (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][10]->push_back(value);
}


void MonitoringObject::fill_eta_matchfail_merged (unsigned int thresh, float value){
  if (thresh > m_ThreshMon.size()) return; //Safeguard against invalid threshold number, Valid thresholds are 1-->6 inclusive
  else m_ThreshMon[thresh][11]->push_back(value);
}
