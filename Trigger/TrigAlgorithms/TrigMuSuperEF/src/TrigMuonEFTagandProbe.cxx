
/*
  In progress Event Filter Z_mu+mu- Online Tag and Probe algorithm
  Authors: JJamieson, University of Glasgow, 09/10/2017, Last edit: 29/01/2018
  

  Summary:-----------------------------------------------------------------------------------------  
  
  Accesses collection of xaod::muons within event and picks out only 'good muons', i.e. muons formed from combined (NOT Stand Alone) tracks including any duplicates and with isolation criteria DeltaR > 0.2 
  
  Preceding Mu22_mu8noL1 trigger can pass more than 2 muon events provided that there are at least one with Pt>22GeV and another with Pt>8GeV, Thus data is trimmed by first applying a 10 GeV cut on pt to remove events with pt below that expected in z_mu+mu- events (10 GeV picked to not be too high as to bias against low momentum Z events and to stay consistent with previous work
  
  >2 muon events are then trimmed to exact pairs by matching all the avaliable opposite charge muons and picking the best fit to the Z mass, applying a +-10 GeV threshold cut around the Z mass to match the eventual pair requirement. 
  
  (Depreciated) Invariant mass and Delta Z0 values are plotted for each good muon pair
  
  LVL 1 trigger information is then accessed for the event and Delta R values for each RoI and muon pair are collected 
  
  For the Tag and probe method a tag muon is defined as any good muon with pt > 10GeV that can be associated with a LVL 1 RoI (DeltaR <0.1 and LVL1 threshold >= threshold 5 (MU20)) and a candidate probe muon is any other muon with pt > 10GeV with the additional requirement on tag and probe pairs that their invariant mass be within +-10GeV from the Z mass 

  For probe muon candidates without a matching L1 RoI: count 1 towards total in all thresholds to avoid biasing for higher efficiencies at pTs lower than threshold acceptance.
  
  (Depreciated) Trigger efficiency plots for kinemetic variables (eta,phi and pt) are produced seperately for each LVL1 threshold by dividing histograms of 'kinematic value vs. number passing probes' by 'kinemetic value vs. number of total probe candidates'. 

  Efficiency plots for three kinematic vairbales (eta,phi and pT) are produced from TProfile objects created from two vectors
  -kinematic values for every probe candidate [e.g: EF_Pt_Total_thr1_b] 
  -1 or 0 for every probe candidate denoting RoI match (1) or no matching L1 info (0)

  Seperate plots for each threshold as well as barrel and endcap where appropriate are produced for all kinemetics and a flat PT cut of 25GeV applied to data in Eta and Phi plots for all thresholds to avoid poor efficiency at to low pT turn on. 

  Eta barrel and endcap statistics are merged to facilitate more sensible plotting of efficiencies but seperated statistics still required for producing 2d plots vs Phi


  
  Questions:-----------------------------------------------------------------------------------------  

  Should I change variables I need but don't want to plot to transient containers that get cleared after every event and then deleted at finalize or can I keep them as monitored variables and just not plot them

  TODO:-----------------------------------------------------------------------------------------  


  Running:-----------------------------------------------------------------------------------------  

  athenaHLT:

  Run on multiple files using: from glob import glob;athenaHLT.py -M -f "glob("/path/to/dir1/star“)+glob("/path/to/dir2/star“)+glob(etc..)" -o HLT_MCV7  -c 'testMCV7=True;HLTOutputLevel=INFO;ServiceMgr.MessageSvc.defaultLimit=9999999;doValidation=True;fpeAuditor=True' -C "topSequence.TrigSteer_HLT.TrigMuonEFTagandProbe.OutputLevel=DEBUG" TriggerRelease/runHLT_standalone.py >& log 

  Output of online histogramming will be in r000...HLT-Histogramming.root


  athena (data):

  athena.py -c 'testMCV7=True;HLTOutputLevel=INFO;ServiceMgr.MessageSvc.defaultLimit=9999999;doValidation=True;fpeAuditor=True;from glob import glob;BSRDOInput=glob("/path/to/dir1/star“)+glob("/path/to/dir2/star“)+glob(etc..)'  TriggerRelease/runHLT_standalone.py ../setupDebugForMuons.py >& log_file


  athena (MC):

/usr/bin/time -f "mem=%K RSS=%M elapsed=%E cpu.sys=%S .user=%U" athena.py -c 'menu="MC_pp_v7";sliceName="muon";ServiceMgr.MessageSvc.defaultLimit=9999999;EvtMax=-1;jp.AthenaCommonFlags.FilesInput=glob("/path/to/dir1/star")+glob("/path/to/dir2/star“)+glob(etc..)' TriggerTest/testCommonSliceAthenaTrigRDO.py ../setupDebugForMuons.py >& log_MC_Debug_test

  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
 */


#include <iomanip>
#include <cmath>
#include "TrigMuonEFTagandProbe.h"

#include "TrigConfHLTData/HLTTriggerElement.h"


//Constructor for new algorithm class inheriting from base FexAlgo class
//ISvcLocator is service locator for shared global services between algorithms (e.g. magnetic field information etc)
TrigMuonEFTagandProbe::TrigMuonEFTagandProbe(const std::string &name, ISvcLocator *pSvcLocator) : 
  FexAlgo(name, pSvcLocator),
  m_debug(false),
  m_verbose(false)

{

  //Use constructor to declare properties and containers

  //Soon to be depreciated - Need Invmass values at event level, should only be 1 value though
  declareMonitoredStdContainer("EF_FS_InvMass",             m_dimuon_invmass,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_FS_DeltaZ",              m_delta_z,                   IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("LVL1_EF_DeltaR_TAG",        m_delta_r_tag,               IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("LVL1_EF_DeltaR_PROBE",      m_delta_r_probepass,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("LVL1_EF_DeltaR_Thresh1",    m_delta_r_thresh1,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("LVL1_EF_DeltaR_Thresh2",    m_delta_r_thresh2,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("LVL1_EF_DeltaR_Thresh3",    m_delta_r_thresh3,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("LVL1_EF_DeltaR_Thresh4",    m_delta_r_thresh4,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("LVL1_EF_DeltaR_Thresh5",    m_delta_r_thresh5,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("LVL1_EF_DeltaR_Thresh6",    m_delta_r_thresh6,           IMonitoredAlgo::AutoClear);
  //End of Depreciation  

  //Soon to depreciate all Passing vectors, replaced by efficiency vectors
  declareMonitoredStdContainer("EF_Eta_Total_thr1_b",       m_eta_total_thr1_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr1_b",     m_eta_passing_thr1_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr1_b",       m_phi_total_thr1_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr1_b",     m_phi_passing_thr1_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr1_b",        m_pt_total_thr1_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr1_b",      m_pt_passing_thr1_b,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr1_e",       m_eta_total_thr1_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr1_e",     m_eta_passing_thr1_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr1_e",       m_phi_total_thr1_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr1_e",     m_phi_passing_thr1_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr1_e",        m_pt_total_thr1_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr1_e",      m_pt_passing_thr1_e,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr1",         m_eta_total_thr1,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr1",       m_eta_passing_thr1,          IMonitoredAlgo::AutoClear);


  declareMonitoredStdContainer("EF_Eta_Total_thr2_b",       m_eta_total_thr2_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr2_b",     m_eta_passing_thr2_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr2_b",       m_phi_total_thr2_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr2_b",     m_phi_passing_thr2_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr2_b",        m_pt_total_thr2_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr2_b",      m_pt_passing_thr2_b,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr2_e",       m_eta_total_thr2_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr2_e",     m_eta_passing_thr2_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr2_e",       m_phi_total_thr2_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr2_e",     m_phi_passing_thr2_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr2_e",        m_pt_total_thr2_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr2_e",      m_pt_passing_thr2_e,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr2",         m_eta_total_thr2,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr2",       m_eta_passing_thr2,          IMonitoredAlgo::AutoClear);


  declareMonitoredStdContainer("EF_Eta_Total_thr3_b",       m_eta_total_thr3_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr3_b",     m_eta_passing_thr3_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr3_b",       m_phi_total_thr3_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr3_b",     m_phi_passing_thr3_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr3_b",        m_pt_total_thr3_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr3_b",      m_pt_passing_thr3_b,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr3_e",       m_eta_total_thr3_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr3_e",     m_eta_passing_thr3_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr3_e",       m_phi_total_thr3_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr3_e",     m_phi_passing_thr3_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr3_e",        m_pt_total_thr3_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr3_e",      m_pt_passing_thr3_e,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr3",         m_eta_total_thr3,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr3",       m_eta_passing_thr3,          IMonitoredAlgo::AutoClear);


  declareMonitoredStdContainer("EF_Eta_Total_thr4_b",       m_eta_total_thr4_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr4_b",     m_eta_passing_thr4_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr4_b",       m_phi_total_thr4_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr4_b",     m_phi_passing_thr4_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr4_b",        m_pt_total_thr4_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr4_b",      m_pt_passing_thr4_b,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr4_e",       m_eta_total_thr4_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr4_e",     m_eta_passing_thr4_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr4_e",       m_phi_total_thr4_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr4_e",     m_phi_passing_thr4_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr4_e",        m_pt_total_thr4_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr4_e",      m_pt_passing_thr4_e,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr4",         m_eta_total_thr4,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr4",       m_eta_passing_thr4,          IMonitoredAlgo::AutoClear);


  declareMonitoredStdContainer("EF_Eta_Total_thr5_b",       m_eta_total_thr5_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr5_b",     m_eta_passing_thr5_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr5_b",       m_phi_total_thr5_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr5_b",     m_phi_passing_thr5_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr5_b",        m_pt_total_thr5_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr5_b",      m_pt_passing_thr5_b,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr5_e",       m_eta_total_thr5_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr5_e",     m_eta_passing_thr5_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr5_e",       m_phi_total_thr5_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr5_e",     m_phi_passing_thr5_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr5_e",        m_pt_total_thr5_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr5_e",      m_pt_passing_thr5_e,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr5",         m_eta_total_thr5,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr5",       m_eta_passing_thr5,          IMonitoredAlgo::AutoClear);


  declareMonitoredStdContainer("EF_Eta_Total_thr6_b",       m_eta_total_thr6_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr6_b",     m_eta_passing_thr6_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr6_b",       m_phi_total_thr6_b,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr6_b",     m_phi_passing_thr6_b,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr6_b",        m_pt_total_thr6_b,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr6_b",      m_pt_passing_thr6_b,         IMonitoredAlgo::AutoClear);

  declareMonitoredStdContainer("EF_Eta_Total_thr6_e",       m_eta_total_thr6_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr6_e",     m_eta_passing_thr6_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Total_thr6_e",       m_phi_total_thr6_e,          IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Passing_thr6_e",     m_phi_passing_thr6_e,        IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Total_thr6_e",        m_pt_total_thr6_e,           IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Passing_thr6_e",      m_pt_passing_thr6_e,         IMonitoredAlgo::AutoClear);
  
  declareMonitoredStdContainer("EF_Eta_Total_thr6",         m_eta_total_thr6,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Passing_thr6",       m_eta_passing_thr6,          IMonitoredAlgo::AutoClear);



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

  ATH_MSG_DEBUG("testphrase TrigMuonEFTagandProbe::hltInitialize()"); 
  ATH_MSG_INFO("testphrase TrigMuonEFTagandProbe::hltInitialize(); building framework for monitored variables"); 

  //Set up frame to hold monitoring vectors in place (There must be a better way of doing this)

  Thresh_Mon.push_back(&m_eta_total_thr1_b);
  Thresh_Mon.push_back(&m_eta_passing_thr1_b);
  Thresh_Mon.push_back(&m_phi_total_thr1_b);
  Thresh_Mon.push_back(&m_phi_passing_thr1_b);
  Thresh_Mon.push_back(&m_pt_total_thr1_b);
  Thresh_Mon.push_back(&m_pt_passing_thr1_b);

  Thresh_Mon.push_back(&m_eta_total_thr1_e);
  Thresh_Mon.push_back(&m_eta_passing_thr1_e);
  Thresh_Mon.push_back(&m_phi_total_thr1_e);
  Thresh_Mon.push_back(&m_phi_passing_thr1_e);
  Thresh_Mon.push_back(&m_pt_total_thr1_e);
  Thresh_Mon.push_back(&m_pt_passing_thr1_e);


  Thresh_Mon.push_back(&m_eta_total_thr2_b);
  Thresh_Mon.push_back(&m_eta_passing_thr2_b);
  Thresh_Mon.push_back(&m_phi_total_thr2_b);
  Thresh_Mon.push_back(&m_phi_passing_thr2_b);
  Thresh_Mon.push_back(&m_pt_total_thr2_b);
  Thresh_Mon.push_back(&m_pt_passing_thr2_b);

  Thresh_Mon.push_back(&m_eta_total_thr2_e);
  Thresh_Mon.push_back(&m_eta_passing_thr2_e);
  Thresh_Mon.push_back(&m_phi_total_thr2_e);
  Thresh_Mon.push_back(&m_phi_passing_thr2_e);
  Thresh_Mon.push_back(&m_pt_total_thr2_e);
  Thresh_Mon.push_back(&m_pt_passing_thr2_e);


  Thresh_Mon.push_back(&m_eta_total_thr3_b);
  Thresh_Mon.push_back(&m_eta_passing_thr3_b);
  Thresh_Mon.push_back(&m_phi_total_thr3_b);
  Thresh_Mon.push_back(&m_phi_passing_thr3_b);
  Thresh_Mon.push_back(&m_pt_total_thr3_b);
  Thresh_Mon.push_back(&m_pt_passing_thr3_b);

  Thresh_Mon.push_back(&m_eta_total_thr3_e);
  Thresh_Mon.push_back(&m_eta_passing_thr3_e);
  Thresh_Mon.push_back(&m_phi_total_thr3_e);
  Thresh_Mon.push_back(&m_phi_passing_thr3_e);
  Thresh_Mon.push_back(&m_pt_total_thr3_e);
  Thresh_Mon.push_back(&m_pt_passing_thr3_e);


  Thresh_Mon.push_back(&m_eta_total_thr4_b);
  Thresh_Mon.push_back(&m_eta_passing_thr4_b);
  Thresh_Mon.push_back(&m_phi_total_thr4_b);
  Thresh_Mon.push_back(&m_phi_passing_thr4_b);
  Thresh_Mon.push_back(&m_pt_total_thr4_b);
  Thresh_Mon.push_back(&m_pt_passing_thr4_b);

  Thresh_Mon.push_back(&m_eta_total_thr4_e);
  Thresh_Mon.push_back(&m_eta_passing_thr4_e);
  Thresh_Mon.push_back(&m_phi_total_thr4_e);
  Thresh_Mon.push_back(&m_phi_passing_thr4_e);
  Thresh_Mon.push_back(&m_pt_total_thr4_e);
  Thresh_Mon.push_back(&m_pt_passing_thr4_e);


  Thresh_Mon.push_back(&m_eta_total_thr5_b);
  Thresh_Mon.push_back(&m_eta_passing_thr5_b);
  Thresh_Mon.push_back(&m_phi_total_thr5_b);
  Thresh_Mon.push_back(&m_phi_passing_thr5_b);
  Thresh_Mon.push_back(&m_pt_total_thr5_b);
  Thresh_Mon.push_back(&m_pt_passing_thr5_b);

  Thresh_Mon.push_back(&m_eta_total_thr5_e);
  Thresh_Mon.push_back(&m_eta_passing_thr5_e);
  Thresh_Mon.push_back(&m_phi_total_thr5_e);
  Thresh_Mon.push_back(&m_phi_passing_thr5_e);
  Thresh_Mon.push_back(&m_pt_total_thr5_e);
  Thresh_Mon.push_back(&m_pt_passing_thr5_e);


  Thresh_Mon.push_back(&m_eta_total_thr6_b);
  Thresh_Mon.push_back(&m_eta_passing_thr6_b);
  Thresh_Mon.push_back(&m_phi_total_thr6_b);
  Thresh_Mon.push_back(&m_phi_passing_thr6_b);
  Thresh_Mon.push_back(&m_pt_total_thr6_b);
  Thresh_Mon.push_back(&m_pt_passing_thr6_b);

  Thresh_Mon.push_back(&m_eta_total_thr6_e);
  Thresh_Mon.push_back(&m_eta_passing_thr6_e);
  Thresh_Mon.push_back(&m_phi_total_thr6_e);
  Thresh_Mon.push_back(&m_phi_passing_thr6_e);
  Thresh_Mon.push_back(&m_pt_total_thr6_e);
  Thresh_Mon.push_back(&m_pt_passing_thr6_e);

  //Soon to be depreciated
  Thresh_Mon.push_back(&m_delta_r_thresh1);
  Thresh_Mon.push_back(&m_delta_r_thresh2);
  Thresh_Mon.push_back(&m_delta_r_thresh3);
  Thresh_Mon.push_back(&m_delta_r_thresh4);
  Thresh_Mon.push_back(&m_delta_r_thresh5);
  Thresh_Mon.push_back(&m_delta_r_thresh6);
  //
  
  //Merged Barrel and Endcap stats for eta plots
  Thresh_Mon.push_back(&m_eta_total_thr1); //79
  Thresh_Mon.push_back(&m_eta_passing_thr1);

  Thresh_Mon.push_back(&m_eta_total_thr2);
  Thresh_Mon.push_back(&m_eta_passing_thr2);

  Thresh_Mon.push_back(&m_eta_total_thr3);
  Thresh_Mon.push_back(&m_eta_passing_thr3);

  Thresh_Mon.push_back(&m_eta_total_thr4);
  Thresh_Mon.push_back(&m_eta_passing_thr4);

  Thresh_Mon.push_back(&m_eta_total_thr5);
  Thresh_Mon.push_back(&m_eta_passing_thr5);

  Thresh_Mon.push_back(&m_eta_total_thr6);
  Thresh_Mon.push_back(&m_eta_passing_thr6);


  
  
  Thresh_Mon.push_back(&m_etaphi_eff_thr1_b);//91
  Thresh_Mon.push_back(&m_pt_eff_thr1_b);  

  Thresh_Mon.push_back(&m_etaphi_eff_thr2_b);
  Thresh_Mon.push_back(&m_pt_eff_thr2_b);

  Thresh_Mon.push_back(&m_etaphi_eff_thr3_b);
  Thresh_Mon.push_back(&m_pt_eff_thr3_b);  

  Thresh_Mon.push_back(&m_etaphi_eff_thr4_b);
  Thresh_Mon.push_back(&m_pt_eff_thr4_b);  

  Thresh_Mon.push_back(&m_etaphi_eff_thr5_b);
  Thresh_Mon.push_back(&m_pt_eff_thr5_b);  

  Thresh_Mon.push_back(&m_etaphi_eff_thr6_b);
  Thresh_Mon.push_back(&m_pt_eff_thr6_b);  


  Thresh_Mon.push_back(&m_etaphi_eff_thr1_e);
  Thresh_Mon.push_back(&m_pt_eff_thr1_e);

  Thresh_Mon.push_back(&m_etaphi_eff_thr2_e);
  Thresh_Mon.push_back(&m_pt_eff_thr2_e);

  Thresh_Mon.push_back(&m_etaphi_eff_thr3_e);
  Thresh_Mon.push_back(&m_pt_eff_thr3_e);

  Thresh_Mon.push_back(&m_etaphi_eff_thr4_e);
  Thresh_Mon.push_back(&m_pt_eff_thr4_e);

  Thresh_Mon.push_back(&m_etaphi_eff_thr5_e);
  Thresh_Mon.push_back(&m_pt_eff_thr5_e);

  Thresh_Mon.push_back(&m_etaphi_eff_thr6_e);
  Thresh_Mon.push_back(&m_pt_eff_thr6_e);



  Thresh_Mon.push_back(&m_eta_eff_thr1);//115 
  Thresh_Mon.push_back(&m_eta_eff_thr2); 
  Thresh_Mon.push_back(&m_eta_eff_thr3); 
  Thresh_Mon.push_back(&m_eta_eff_thr4); 
  Thresh_Mon.push_back(&m_eta_eff_thr5); 
  Thresh_Mon.push_back(&m_eta_eff_thr6); 


  ATH_MSG_INFO("testphrase TrigMuonEFTagandProbe Initialization completed successfully");
 
  return HLT::OK;
}



HLT::ErrorCode TrigMuonEFTagandProbe::hltFinalize()
{
  ATH_MSG_DEBUG("testphrase TrigMuonEFTagandProbe::hltFinalize()"); 

  //Might need to formally delete some transient variables here, depends on how they are dealt with  

  return HLT::OK;
}


//inputTE and TEout are passed to algorithm from Trigger menu
HLT::ErrorCode TrigMuonEFTagandProbe::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout) 
{
  // Conditional debug flags for when debug output requires extra compute
  m_debug = msgLvl() <= MSG::DEBUG; // MSG levels are 1-7, VERBOSE,DEBUG,INFO,WARNINGS,ERROR,FATAL,ALWAYS
  m_verbose = msgLvl() <= MSG::VERBOSE;


  //Reset temporary containers
  
  m_good_muons.clear(); //holds all combined muons
  //Potentially add more here

  ATH_MSG_DEBUG("testphrase TrigMuonEFTagandProbe::hltExecute()"); 


  //DEBUG to check if Trigger events are passing through algorithm or not
  if (m_debug){
    std::string teInLabel;
    std::string teOutLabel;
    TrigConf::HLTTriggerElement::getLabel( inputTE->getId(), teInLabel );
    TrigConf::HLTTriggerElement::getLabel( TEout->getId(), teOutLabel );
    
    ATH_MSG_DEBUG("testphrase TrigMuonEFTagandProbeExecute() chain Trigger Event input ID=" << inputTE->getId() << " Trigger Event output ID=" << TEout->getId() << " Input Label=" << teInLabel << " Output Label=" << teOutLabel);
  }



  //Access LVL1 trigger ROI information-----------------------------------------------------------------------

  
  //Have to access directly from Storegate as AllTE combiner algorithms remove regular access back to seeding RoIs
  //Check for LVL1 muon RoI information in current event first, if there is no info then we shouldn't bother calculating anything else and should move to next event 

  if(evtStore()->retrieve(m_l1_muon_RoIs).isFailure()){
    ATH_MSG_ERROR("testphrase Problem retrieving L1 muon ROI");
    return HLT::MISSING_FEATURE; //CHECK change to NO_LVL1_ITEMS?
  }
  
  if(m_l1_muon_RoIs->empty()){ //Save compute by killing if there is no L1 information (MOVE ABOVE)
    ATH_MSG_ERROR("testphrase L1 RoI size = 0, moving to next event");
    return HLT::OK; 
  }
  
  ATH_MSG_INFO("testphrase Number of L1 RoIs found = "<<m_l1_muon_RoIs->size());    




  // Get muonContainer linked to inputTE--------------------------------------------------------------------

  const xAOD::MuonContainer*  muonContainer=0; 

  // getFeature takes a trigger element (inputTE) and a target feature (MuonContainer) and fills the target with the feature found in the trigger element, Output is a HLT::code, but OK does NOT mean a matching, non zero feature was found just that the feature exists within the trigger element
 
  if(getFeature(inputTE, muonContainer)!=HLT::OK) {  //If getfeature fails
    ATH_MSG_DEBUG("testphrase no xAOD::MuonContainer Feature found");
    return HLT::MISSING_FEATURE;
  } 

  else { //If get feature succeeds 
    if (!muonContainer) { // if muonContainer entry is null
      ATH_MSG_DEBUG("testphrase null xAOD::MuonContainer Feature found");
      return HLT::MISSING_FEATURE;
    } 
    
    ATH_MSG_DEBUG("testphrase xAOD::MuonContainer found with size " << muonContainer->size());
  }
  


  // Initialise counters and temporary value containers
  
  float invMass_dimuon=0.0; 
  float deltaR=0.0; 
  float TaPdeltaR=0.0; 
  float deltaZ=0.0; //Soon to be depreciated
  unsigned int tag_thresh=0;
  unsigned int probe_thresh=0;

  struct TaPMuon tagmuon, probemuon; //TaP Muon structures to group muon and RoI values into one object
  

  // loop over EF muons within event --------------------------------------------------------------------


  // CHECK Might want to remove call to trackparticle because we can use muon->muonType()==0 to check for combined muon
  // Would not be able to output chi^2 value associated to muon tracking in this case though

  for(auto muon : *muonContainer) {
                              
    if (!muon) { // Check if muon object is null
      ATH_MSG_DEBUG("testphrase No xAOD::Muon found.");
      continue; //move to next muon
    } 
    else {

      if (muon->primaryTrackParticle()) { //was there a muon in this RoI? (check muon hasn't been stripped of information) 
                                                                                
        const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::CombinedTrackParticle); //Checking muon is good (i.e. EF Combined muon NOT EF stand alone muon)


	// 10 GeV minimum pT cut applied-----------------------------------------------------------------


        if (!tr || tr->pt()/CLHEP::GeV < 10.0) { // if this muon has NOT been formed from a combined track > 10GeV

          ATH_MSG_DEBUG("testphrase Bad muon is of type: " << muon->muonType() << " With 0=combined, 1=MuonSA, 2=SegmentTagged, 3=CaloTagged, 4=SiliconAssociatedForwardMuon, Abs pt = " << muon->pt()/CLHEP::GeV << " GeV. Charge=" << muon->charge() << " Psuedorapidity = " << muon->eta() << " Phi = " << muon->phi());
          continue;
        } 

	//Collect good muons------------------------------------------------------------------------------

	else { //Current muon is formed from combined track
	  ATH_MSG_DEBUG("testphrase Retrieved muon of type " << muon->muonType() << ": Combined track with abs pt " << tr->pt()/CLHEP::GeV << " GeV. Charge=" << tr->charge() << " match chi2 = " << tr->chiSquared() << " Psuedorapidity = " << tr->eta() << " Phi = " << tr->phi());
	  
	  m_good_muons.push_back(muon);
	  mu_count++; // Soon to be depreciated?
	}
      }
    }
  }
  


  if (m_good_muons.size()<2){
    ATH_MSG_WARNING("testphrase Less than 2 combined muons before trimming so event will be ignored, moving to next event");
    return HLT::OK;
  }

  if (m_good_muons.size()>2 || (m_good_muons)[0]->charge() == (m_good_muons)[1]->charge())
    {
      ATH_MSG_DEBUG("testphrase >2 muons found or matching charge found, no. of \"good\" muons = " << m_good_muons.size());
      trim_container(m_good_muons); //Performs trimming of muon container reducing to size = 2
      
      if (m_good_muons.size()!=2){
	ATH_MSG_WARNING("testphrase More/Less than 2 good muons after trimming so further event analysis will be ignored, moving to next event");
	return HLT::OK;
      }
      
      ATH_MSG_DEBUG("testphrase container trimmed, no. of good muons = " << m_good_muons.size());
    }
  


  //Checks isolation criteria for muon pair, require deltaR > 0.2 between the two muons in order to keep probe muons unbiased from the event trigger

  TaPdeltaR=sqrt(pow(m_good_muons[0]->phi()-m_good_muons[1]->phi(),2) + pow(m_good_muons[0]->eta()-m_good_muons[1]->eta(),2));

  if (TaPdeltaR <= 0.2){
    ATH_MSG_WARNING("testphrase Tag and Probe not sufficiently isolated for unbiased efficiency; TaPdeltaR = "<< TaPdeltaR <<", moving to next event");
    return HLT::OK;
  }


  
  //Calculate dimuon invariant mass from 4 momenta
  
  invMass_dimuon = (((m_good_muons)[0]->p4() + (m_good_muons)[1]->p4()).M())/CLHEP::GeV;
    
  m_dimuon_invmass.push_back(invMass_dimuon); //Soon to be depreciated
  
  
  //Soon to be depreciated
  //Get track associated with each particle to calculate Delta z0
  const xAOD::TrackParticle* track1 = m_good_muons[0]->trackParticle(xAOD::Muon::CombinedTrackParticle); 
  const xAOD::TrackParticle* track2 = m_good_muons[1]->trackParticle(xAOD::Muon::CombinedTrackParticle); 
  deltaZ=abs(track1->z0()-track2->z0());
  m_delta_z.push_back(deltaZ);
  //
  
  ATH_MSG_DEBUG("testphrase final size of good muon container = " << m_good_muons.size());
  ATH_MSG_DEBUG("testphrase Dimuon Invariant Mass from 4mom  = " << invMass_dimuon);
  ATH_MSG_DEBUG("testphrase Dimuon delta Z = " << deltaZ); //soon to be depreciated
  


  //Perform Tag L1 matching to produce Tag and Probe pairs-------------------------------------------------------------

  

  //Define a vector of pairs of TaPmuon structure object and Probe muons i.e. tag and probe pairs
  std::vector<std::pair <TaPMuon,const xAOD::Muon*> > TaP;

  LVL1::RecMuonRoI* sharedRoItest=0; //don't bother trying to match muons to same RoI as isolation criteria already applied

  for (unsigned int i=0;i<m_good_muons.size();i++){ //loop over good muons
    
    for (auto roi : *m_l1_muon_RoIs){
      //For each muon calculate all Delta R values in scope

      deltaR=sqrt(pow(roi->phi()-m_good_muons[i]->phi(),2) + pow(roi->eta()-m_good_muons[i]->eta(),2));

      ATH_MSG_DEBUG("testphrase DeltaR value = " << deltaR); 
      
      tag_thresh=roi->getThresholdNumber();

      ATH_MSG_DEBUG("testphrase Threshold_ALL value = " << tag_thresh); 
      

      //Tag Muon criteria

      if (abs(91.1876-invMass_dimuon)<10.0 && deltaR<0.1 && roi!=sharedRoItest && tag_thresh>=5) {
	sharedRoItest=roi; // CHECK Not sure if this biases towards higher efficiency or not


	ATH_MSG_DEBUG("testphrase Threshold_Tag value = " << tag_thresh); 

	m_delta_r_tag.push_back(deltaR); //DeltaR_tag value

	tagmuon.Mu = m_good_muons[i]; //Tag muon
	tagmuon.DR = deltaR; //Tag muon DeltaR
	tagmuon.Thresh = tag_thresh; // Tag muon matched L1 threshold number
	tagmuon.RoI = roi; //Tag muon match L1 RoI

	//construct Tag and Probe pair including extra tag muon information
	TaP.push_back(std::make_pair(tagmuon,m_good_muons[1-i])); 
	break; //don't need to check other RoIs once a match is found
      }
	
    }
  }
  ATH_MSG_DEBUG("testphrase NUMBER of tag and probe pairs = " << TaP.size()); 
  


  //Perform Probe L1 matching to fill Total and Passing histograms for each threshold-----------------------------


  for (unsigned int i=0;i<TaP.size();i++){ //Loop over each tag and Probe pair
    total++;
    //if Probe is also a tag then Tag matching isn't required, go straight to filling kinemetics
    if (TaP[i].second==(TaP[1-i].first).Mu){ 
      hit++;
      m_delta_r_probepass.push_back((TaP[1-i].first).DR);
      match_thresh(TaP[1-i].first,Thresh_Mon);//Function to fill kinemetics for different probe thresholds inclusively (probe muon info,referenced vector of threshold histograms)
      continue;
    }

    //If probe isn't a tag then loop over RoIs and look for a L1 match that's different from the Tag L1
    for (unsigned int j=0;j<m_l1_muon_RoIs->size();j++){
      deltaR=sqrt(pow((*m_l1_muon_RoIs)[j]->phi()-(TaP[i].second)->phi(),2) + pow((*m_l1_muon_RoIs)[j]->eta()-(TaP[i].second)->eta(),2));
      if (deltaR<0.1 && (*m_l1_muon_RoIs)[j]!=(TaP[1-i].first).RoI){ //If probe passes LVL1 match criteria and TaP LVL1 RoI's aren't the same
	hit++;
	m_delta_r_probepass.push_back(deltaR);

	probe_thresh=(*m_l1_muon_RoIs)[j]->getThresholdNumber();

	ATH_MSG_DEBUG("testphrase Threshold_Probe value = " << probe_thresh);

	probemuon.Mu = TaP[i].second; //Probe Muon
        probemuon.DR = deltaR; //Probe muon DeltaR
        probemuon.Thresh = probe_thresh; //Probe muon matched L1 threshold number


	match_thresh(probemuon,Thresh_Mon); //Fill kinematics
	break;
      } 

      //If Probe muon cannot be L1 matched
      if (j==(m_l1_muon_RoIs->size()-1)){

	match_thresh(TaP[i].second,Thresh_Mon); //Fill only Total kinematics
	
      }
    }
  }


  //Soon to be depreciated
  ATH_MSG_DEBUG("testphrase RUNNING TOTAL = " << total << " HITS = " << hit); 
  ATH_MSG_DEBUG("testphrase RUNNING TOTAL number of muons processed = " << mu_count); 

  return HLT::OK;
  
}

//If Probe is successfully matched
void TrigMuonEFTagandProbe::match_thresh (TaPMuon tap ,std::vector<std::vector <float>* >& thresh_mon){

  //Barrel
  if (abs(tap.Mu->eta())<1.05){
    for (unsigned int i=0;i<tap.Thresh;i++){
      //thresh_mon[i*12]->push_back(tap.Mu->eta()); //Dirty fix to plot eta not split by barrel and encap
      //thresh_mon[1+i*12]->push_back(tap.Mu->eta());
      if ((tap.Mu->pt()/CLHEP::GeV) > 25.0){
	thresh_mon[i*12]->push_back(tap.Mu->eta()); //eta barrel total
	thresh_mon[1+i*12]->push_back(tap.Mu->eta()); //eta barrel passed
	thresh_mon[2+i*12]->push_back(tap.Mu->phi()); //phi barrel total
	thresh_mon[3+i*12]->push_back(tap.Mu->phi()); //phi barrel passed 
	thresh_mon[78+i*2]->push_back(tap.Mu->eta()); //eta merged total
	thresh_mon[79+i*2]->push_back(tap.Mu->eta()); //eta merged passed
       
	thresh_mon[114+i]->push_back(1.0); //eta merged eff
	thresh_mon[90+i*2]->push_back(1.0); //etaphi_b eff
	

      }
      thresh_mon[4+i*12]->push_back(tap.Mu->pt()/CLHEP::GeV); //pT barrel Total
      thresh_mon[5+i*12]->push_back(tap.Mu->pt()/CLHEP::GeV); //pT barrel Passed

      
      thresh_mon[91+i*2]->push_back(1.0); //pt_b merged eff
      

      thresh_mon[72+i]->push_back(tap.DR); //DR merged 

    }
  }
  //Endcap
  else {
    for (unsigned int i=0;i<tap.Thresh;i++){
      if ((tap.Mu->pt()/CLHEP::GeV) > 25.0){
	thresh_mon[6+i*12]->push_back(tap.Mu->eta()); //eta endcap total
	thresh_mon[7+i*12]->push_back(tap.Mu->eta()); //eta endcap passed
	thresh_mon[8+i*12]->push_back(tap.Mu->phi()); //phi endcap total
	thresh_mon[9+i*12]->push_back(tap.Mu->phi()); //phi endcap passed
	thresh_mon[78+i*2]->push_back(tap.Mu->eta()); //eta merged total
	thresh_mon[79+i*2]->push_back(tap.Mu->eta()); //eta merged passed

	
	thresh_mon[114+i]->push_back(1.0); //eta merged eff
	thresh_mon[102+i*2]->push_back(1.0); //etaphi_e merged eff
	  

      }
      thresh_mon[10+i*12]->push_back(tap.Mu->pt()/CLHEP::GeV); //pT endcap Total
      thresh_mon[11+i*12]->push_back(tap.Mu->pt()/CLHEP::GeV); //pT endcap Passed
      
      
      thresh_mon[103+i*2]->push_back(1.0); //pt_e merged eff

      thresh_mon[72+i]->push_back(tap.DR); //DR merged
    }
  }
  
}

//If Probe muon cannot be matched
void TrigMuonEFTagandProbe::match_thresh (const xAOD::Muon* muon ,std::vector<std::vector <float>* >& thresh_mon){
  unsigned int thresh=6;

  /*
  if (floor(muon->pt()/CLHEP::GeV)<=4.0){
    thresh=1;
  }
  else if (floor(muon->pt()/CLHEP::GeV)>4.0 && floor(muon->pt()/CLHEP::GeV)<=6.0){
    thresh=2;
  }
  else if (floor(muon->pt()/CLHEP::GeV)>6.0 && floor(muon->pt()/CLHEP::GeV)<=10.0){
    thresh=3;
  }
  else if (floor(muon->pt()/CLHEP::GeV)>10.0 && floor(muon->pt()/CLHEP::GeV)<=11.0){
    thresh=4;
  }
  else if (floor(muon->pt()/CLHEP::GeV)>11.0 && floor(muon->pt()/CLHEP::GeV)<=20.0){
    thresh=5;
  }
  else{
    thresh=6;
  }
  */

  //Only fill total histograms
  if (abs(muon->eta())<1.05){
    for (unsigned int i=0;i<thresh;i++){
      //thresh_mon[i*12]->push_back(muon->eta());
      if ((muon->pt()/CLHEP::GeV) > 25.0){
	thresh_mon[i*12]->push_back(muon->eta()); //eta barrel total
	
	thresh_mon[2+i*12]->push_back(muon->phi()); //phi barrel total 

	thresh_mon[78+i*2]->push_back(muon->eta()); //eta merged total

	
	thresh_mon[114+i]->push_back(0.0); //eta merged eff
	thresh_mon[90+i*2]->push_back(0.0); //etaphi_b eff
	


      }
      thresh_mon[4+i*12]->push_back(muon->pt()/CLHEP::GeV); //pT barrel Total


      
      thresh_mon[91+i*2]->push_back(0.0); //pt_b merged eff
      
      
    }
  }
  else {
    for (unsigned int i=0;i<thresh;i++){
      if ((muon->pt()/CLHEP::GeV) > 25.0){
	thresh_mon[6+i*12]->push_back(muon->eta()); //eta endcap total
	
	thresh_mon[8+i*12]->push_back(muon->phi()); //phi endcap total

	thresh_mon[78+i*2]->push_back(muon->eta()); //eta merged total

	thresh_mon[114+i]->push_back(0.0); //eta merged eff
	thresh_mon[102+i*2]->push_back(0.0); //etaphi_e merged eff
	
      }
      thresh_mon[10+i*12]->push_back(muon->pt()/CLHEP::GeV); //pT endcap Total

      
      thresh_mon[103+i*2]->push_back(0.0); //pt_e merged eff
      
	
    }
  }
}




//Function to resolve cases of >2 muons within muon container
void TrigMuonEFTagandProbe::trim_container (std::vector<const xAOD::Muon*>& good_muons){

  //Splits good_muon vector into two vectors by charge of muons
  //Loop through both vectors comparing Invariant Mass of element pairs
  //if abs(91.1876-invariant mass) is below a given threshold (10 GeV) then write this as a key into a map with the second loop muon as the element
  //at last element of second loop push back first map element and first loop muon pair into good_muons vector then remove muon from second loop to avoid producing two pairs with a shared muon (Doesn't account for first loop muon being being a potential pair with multiple second loop muons and the order removing a valid pair but multiple z-mumu per event is so unlikely this is likely null

  //CHECK should I define these globally (in h file) and just clear at every call of function( is there a difference, how exactly are function variables working here?)

  std::vector<const xAOD::Muon*> posvec;
  std::vector<const xAOD::Muon*> negvec;

  std::map<const double,std::pair <const xAOD::Muon*,unsigned int> > comparisonmap;

  for(auto muon : good_muons) {
    if (muon->charge()>0){
      posvec.push_back(muon);
    }    
    else{
      negvec.push_back(muon);
    }
  }
  
  good_muons.clear();

  if (posvec.empty() || negvec.empty()){
    ATH_MSG_INFO("testphrase (trim_container) only found muons of one sign, clearing container and moving on"); 
    return;
  }

  for(auto muon1 : posvec) {
    comparisonmap.clear();
    for (unsigned int j=0;j<negvec.size();j++){
      double tempinvmass = abs(91.1876-((muon1->p4() + negvec[j]->p4()).M())/CLHEP::GeV);
      if (tempinvmass < 10){ // Cut to match condition for valid tag and probe pair
	comparisonmap[tempinvmass] = std::make_pair(negvec[j],j); //maps invmass key to neg muon  and neg muon iterator
      }
      if (j == (negvec.size()-1)){ //Once looped through all muons then go back and check the comparison map
	if (comparisonmap.size()>0){ //if condition stops cbegin() being called on an empty map returning NAN as a value
	 
	  //Find the smallest tempinvmass key value using automatic sorting of maps, use this to pick best muon pair
	  good_muons.push_back(((*(comparisonmap.cbegin())).second).first); //Push back muon 2 (cbegin() always gives pointer)
	  good_muons.push_back(muon1); //Push back muon 1
	  
	  ATH_MSG_DEBUG("testphrase (trim_container) first best match found, removing all other potential pairs");
	  return;
	  //ATH_MSG_DEBUG("testphrase (trim_container) best match found, current negvec size = "<<negvec.size()); 
	  //negvec.erase(negvec.begin()+((*(comparisonmap.cbegin())).second).second); //Use neg muon iterator to remove already paired neg muon so it doesn't get double paired
	  //ATH_MSG_DEBUG("testphrase (trim_container) removing best match, new negvec size = "<<negvec.size()); 
	}	
      }
    }
  }
}
