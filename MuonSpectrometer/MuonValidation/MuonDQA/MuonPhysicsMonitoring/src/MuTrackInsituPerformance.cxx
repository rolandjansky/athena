/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////
// Authors:  I.Nomidis (AUTh) - N. Benekos(Illinois)
// MuonDQA
// Feb. 2009
//
// DESCRIPTION:
// Subject: 	This class does a full Insitu-Performance of the MuonSpectrometer
//		reconstruction efficiency, Inner-Tracker-Reconstruction Efficiency and
//		Trigger Efficiency
//		It is derivated from MuonPhysicsAnalysisMonitoring
///////////////////////////////////////////////////////////////////////////

#include "MuonPhysicsMonitoring/MuTrackInsituPerformance.h"
#include "AnalysisTools/AnalysisTools.h" 

#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"

static const double twoPi = 6.28318;
 
MuTrackInsituPerformance::MuTrackInsituPerformance( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator/TrackToVertexIPEstimator"),
   //   m_trackSumTool("Trk::TrackSummaryTool/InDetTrackSummaryTool"),
   //m_muonIsolationTool("MuonIsolationTool/MuonIsolationTool"),
   m_analysisTools( "AnalysisTools", this ),
   //m_isolationTool("TrackIsolationTool"),
   m_TrkOrTrackParticle("TrackParticle"),
   m_trk_pttot(0),
   m_trk_pttruth(0),
   m_trk_ptpassed(0),
   m_trk_phitot(0),  
   m_trk_phitruth(0),
   m_trk_phipassed(0),      
   m_trk_etatot(0),
   m_trk_etatruth(0),
   m_trk_etapassed(0),                  
   m_TagAndProbe_Charge(0),
   m_TagAndProbe_Cut_Flow(0),
   m_TagAndProbe_ZMass(0),
   m_TagAndProbe_diMuonMass(0),
   m_TagAndProbe_DeltaPhi(0),
   m_TagAndProbe_DeltaR(0),
   m_TagAndProbe_NCandidates(0),
   m_TagAndProbe_Pt(0), 
   m_Tag_TrackPt(0),
   m_Tag_TrackEta(0),
   m_Tag_RelTrkIso(0),
   m_Tag_RelCaloIso(0),
   m_Tag_RelJetIso(0),
   m_Tag_AbsTrkIso(0),
   m_Tag_AbsCaloIso(0),
   m_Tag_AbsJetIso(0),
   m_Tag_NCandidates(0), 
   m_Probe_TrackPt(0),
   m_Probe_TrackEta(0),
   m_Probe_RelTrkIso(0),
   m_Probe_RelCaloIso(0),
   m_Probe_RelJetIso(0),
   m_Probe_AbsTrkIso(0),
   m_Probe_AbsCaloIso(0),
   m_Probe_AbsJetIso(0),
   m_Probe_NCandidates(0),
   m_ProbeTrk_Cut_Flow(0),
   m_ProbeTrk_TrackPt(0),
   m_ProbeTrk_TrackIso(0),
   m_ProbeTrk_NhitsSCT(0),
   m_ProbeTrk_NhitsPIX(0),
   m_ProbeTrk_d0(0),
   m_ProbeTrk_pT(0),
   m_ProbeTrk_chi2ndf(0),   
   m_mutrk_BLayerSharedHits(0),
   m_mutrk_PixelHits(0),
   m_mutrk_PixelLayers(0),
   m_mutrk_SiHits(0),
   m_mutrk_TRTHits(0), 
   m_mutrk_d0(0),
   m_mutrk_chi2(0),
   m_mutrk_relIso(0),
   m_ptcone20_vs_etcone20(0),   
   m_mutrk_relIso_vs_pT(0),   
   m_Probe_RelTrkIso_vs_pT(0), 
   m_CaloIsol(false)
{
  declareProperty( "doMCTruth",          	m_doTruth);
  declareProperty( "InnerTrackContainer" ,      m_InnerTrackContainer );
  declareProperty( "MuonSpectrometerContainer", m_MuonSpectrometerTrackContainer );
  declareProperty( "MuonContainer",	        m_MuonContainer );
  declareProperty( "ElectronContainer",         m_ElectronContainer);
  declareProperty( "JetContainer",              m_JetContainer);
  declareProperty( "TruthParticleContainer",    m_TruthParticleContainer);
  declareProperty( "TrackParticleTruthCollection", m_TrackParticleTruthCollection );
  declareProperty( "McEventCollection",         m_McEventCollection );

  declareProperty( "doMuonType",                m_doMuonTypeCut=true);
  declareProperty( "muonType",                  m_muonType="combined");
  declareProperty( "muonDeltaPhiCut",	        m_cut_deltaPhi = 0);
  declareProperty( "diMuonMassWindow",	        m_cut_diMuonMass = 0);
  declareProperty( "tagPtCut",                  m_cut_tagPt = 0);
  declareProperty( "probePtCut" ,               m_cut_probePt = 0); //preselection pt-cut on probes
  declareProperty( "muonEtaCut",                m_cut_muonEta = 0); //preselection eta-cut on probes 
  declareProperty( "muonPairSign",              m_cut_charge=-1); //keep OS pairs
  declareProperty( "isolationJet",              m_cut_isolationJet=0.);
  declareProperty( "isolationTrkIso",           m_cut_isolationTrk=0.); //relative Track Isolation: Sum(pt)/pt
  declareProperty( "isolationET",               m_cut_isolationET=0.);
  
  declareProperty("TagmuProbeTrk_deltaRMatchCut", m_DRTagProbeMatchCut=0.3);
 
  // Calorimeter Isolation 
  declareProperty("EnergyIsoConeSize",          m_energyIsoCone = 0.4);
  declareProperty("TrackEIsoBarrelCut",         m_eIsoBarrelCut = 15000.);
  declareProperty("TrackEIsoPtRatioBarrelCut",  m_eIsoPtRatioBarrelCut=0.4);
  declareProperty("TrackEIsoTransitionCut",     m_eIsoTransitionCut=8000.);
  declareProperty("TrackEIsoPtRatioTransitionCut", m_eIsoPtRatioTransitionCut=0.1);
  declareProperty("TrackEIsoEndCapCut",         m_eIsoEndCapCut = 12000.);
  declareProperty("TrackEIsoPtRatioEndCapCut",  m_eIsoPtRatioEndCapCut = 0.2);

  //isolation
  declareProperty( "isoConeMax",                m_isoConeMax=0.);
  declareProperty( "isoConeMin",                m_isoConeMin = 0);

  //track quality cuts
  declareProperty( "TrkIsoCone",                m_trk_coneIso=0.);
  
  declareProperty( "TrkPt",                     m_trk_pt=0.);
  declareProperty( "TrkRelTrkIso",              m_trk_relTrkIso = 0);
  declareProperty( "TrkNhitsSCT",               m_trk_nhitsSCT=1);
  declareProperty( "TrkNhitsPIX",               m_trk_nhitsPix=2);
  declareProperty( "TrkImpactPar",              m_trk_d0 = 0);
  declareProperty( "TrkChi2Ndf",                m_trk_chi2ndf = 0);
  declareProperty( "doTrkMuonId",               m_doProbeTrkMuonId=true);

  //Trigger decision
  declareProperty( "useTrigger",                m_doTrigger);
  
  declareProperty("CaloIsol",                   m_CaloIsol);

 // declareProperty( "IsolationTool",             m_isolationTool);
  //declareProperty( "MuonIsolationTool",         m_muonIsolationTool );
  //  declareProperty( "TrkSummaryTool",            m_trackSumTool);
  declareProperty( "TrackToVertexIPEstimator",  m_trackToVertexIPEstimator, "tool for IP estimator");
  declareProperty("vxContainerName",            m_vxContainerName="VxPrimaryCandidate");

  declareProperty( "binsEta",                   m_binsEta );
  declareProperty( "binsPhi",                   m_binsPhi );
  declareProperty( "binsPt",                    m_binsPt );

}
 
//======================================================================================//
/**  Initialize */
//======================================================================================//
StatusCode MuTrackInsituPerformance::initialize() 
{ 
  ATH_MSG_DEBUG( "initialize()" );
  StatusCode sc = ManagedMonitorToolBase::initialize(); 
  if(!sc.isSuccess()) return StatusCode::SUCCESS; 

  /** Initialize tools and services */
  if ( service("StoreGateSvc", m_storeGate).isFailure() ) {
    ATH_MSG_WARNING( "Unable to retrieve pointer to StoreGateSvc" );
    return StatusCode::SUCCESS;
  }

  /// get a handle on the analysis tools
  IToolSvc* toolSvc;
  if (service("ToolSvc",toolSvc).isFailure()) {
    ATH_MSG_DEBUG( " Can't get ToolSvc" );
    return StatusCode::SUCCESS;
  }
  else ATH_MSG_DEBUG( " Retrieved tool " << toolSvc ); 

  if (service("THistSvc", m_thistSvc).isFailure()) {
    ATH_MSG_DEBUG( "Unable to retrieve pointer to THistSvc" );
    return StatusCode::SUCCESS;
  }
  else ATH_MSG_DEBUG( " Retrieved tool " << m_thistSvc ); 

  if ( m_doTrigger ) {
    //m_all   = m_trigDecTool->getChainGroup(".*");
    //m_allL1 = m_trigDecTool->getChainGroup("L1_MU.*");
    //m_allL2 = m_trigDecTool->getChainGroup("L2_.*");
    //m_allEF = m_trigDecTool->getChainGroup("EF_.*");
  }  
  /* Get the TrackSummaryTool only if a Track (even....) */
  /*
    if (m_TrkOrTrackParticle == "Track") {
       
    if ( m_trackSumTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve tool. Will not use it" << m_trackSumTool);
    //return StatusCode::FAILURE;
    } else {
    ATH_MSG_INFO("Working with Track. Retrieved tool " << m_trackSumTool);
    }
    } else if (m_TrkOrTrackParticle == "TrackParticle") {
    ATH_MSG_INFO("Working with TrackParticle. Tool not necessary : " << m_trackSumTool);
    } else {
    ATH_MSG_ERROR("Wrong value for takeSummaryAndPerigeeFrom. Only Track or TrackParticlee allowed"); 
    }
  
    sc = m_trackSumTool.retrieve();
    if ( sc.isFailure() )  {
    ATH_MSG_ERROR( "Failed to retrieve tool " << m_trackSumTool);
    return StatusCode::FAILURE;
    } 
    else ATH_MSG_INFO( "Retrieved tool " << m_trackSumTool );
  */
  if ( m_trackToVertexIPEstimator.retrieve().isFailure() )  {
    ATH_MSG_DEBUG( "Failed to retrieve tool " << m_trackToVertexIPEstimator );
    return StatusCode::SUCCESS;
  }
  else ATH_MSG_DEBUG( "Retrieved tool " << m_trackToVertexIPEstimator );

#if 0
  // Muon Isolation tool
  if (m_muonIsolationTool.retrieve().isFailure() )
    {
      ATH_MSG_DEBUG("Could not get " << m_muonIsolationTool.typeAndName() );
      return StatusCode::SUCCESS;       
    }
  else ATH_MSG_DEBUG( "Retrieved MuonIsolation tool " << m_muonIsolationTool.typeAndName() );
#endif

  // Track Isolation tool
 // if ( m_isolationTool.retrieve().isFailure() ) {
 //   ATH_MSG_DEBUG("Can't get a handle on the Isolation Tools");
 //   return StatusCode::SUCCESS;
 // }
 // else ATH_MSG_DEBUG( "Retrieved TrackIsolation tool " << m_isolationTool );

  /// get a handle on the analysis tools 
  if ( m_analysisTools.retrieve().isFailure() ) {
    ATH_MSG_DEBUG("Can't get handle on analysis tools");
    return StatusCode::SUCCESS;
  }
  else ATH_MSG_DEBUG( "Retrieved analysis tool " << m_analysisTools );
 
 
  /*  
  //get a handle on the isolation tools
  sc = service("ToolSvc", m_tools);
  if ( sc.isFailure() || m_tools == 0 ) {
  ATH_MSG_ERROR( "Unable to retrieve tool service");
  return StatusCode::FAILURE;
  }
  if ( m_tools ) {
  IAlgTool* ptool;
  sc = m_tools->retrieveTool("TrackIsolationTool", ptool);
  if ( sc.isFailure() || ptool == 0 ) {
  ATH_MSG_ERROR( "Unable to retrieve track isolation tool! ");
  return StatusCode::FAILURE;
  } else {
  m_isolationTool = dynamic_cast<TrackIsolationTool*>(ptool);
  if ( ! m_isolationTool ) {
  ATH_MSG_ERROR(  "Unable to cast track isolation tool: ");
  return StatusCode::FAILURE;
  }
  }
  }
  */  

  nEvents = 0;
  nZReco = 0;  
  nZMatch = 0;  
  nZTruth = 0;  

  return StatusCode::SUCCESS;
}

//=============================================================================================================
StatusCode MuTrackInsituPerformance::bookHistogramsRecurrent()
{
  ATH_MSG_DEBUG( "in bookHistograms()" );
  
  StatusCode sc = StatusCode::SUCCESS;
  
  if(newEventsBlock){}
  if(newLumiBlock){}
  if(newRun) {
   
    //////////////////////////////////////////////////////////////////
    std::string m_muonphysicsmonitoring_path = "Muon/MuonPhysicsMonitoring/Zmutrk/";
    MonGroup muonphysmon_tagandprobecut( this, m_muonphysicsmonitoring_path+"TagAndProbe_Cut_Histograms", run, ATTRIB_UNMANAGED );
 
    ///// Tag ////
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_TrackPt = TH1F_LW::create("m_Tag_TrackPt", "Tag - Track Pt [MeV]", 100,0.,100000.) );
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_TrackEta = TH1F_LW::create( "m_Tag_TrackEta", "Tag - Track Eta", 54, -2.7, 2.7 ));    
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_AbsCaloIso = TH1F_LW::create( "m_Tag_AbsCaloIso", "Tag - Abs Calo Isolation (E_{T}) [MeV]", 100,0.,50000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_RelCaloIso = TH1F_LW::create( "m_Tag_RelCaloIso", "Tag - Rel Calo Isolation (E_{T}/p_{T})", 100,0.,5. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_AbsJetIso = TH1F_LW::create( "m_Tag_AbsJetIso", "Tag - Abs Jet Isolation (p^{Jet}_{T}) [MeV]", 100,0.,50000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_RelJetIso = TH1F_LW::create( "m_Tag_RelJetIso", "Tag - Rel Jet Isolation (p^{Jet}_{T}/p_{T})", 100,0.,5. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_AbsTrkIso = TH1F_LW::create( "m_Tag_AbsTrkIso", "Tag - Abs Track Isolation (#Sigma p_{T} in cone/) [MeV]", 100,0.,50000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_RelTrkIso = TH1F_LW::create( "m_Tag_RelTrkIso", "Tag - Rel Track Isolation (#Sigma p_{T} in cone/p_{T}) ", 100,0.,5. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_NCandidates = TH1F_LW::create( "m_Tag_NCandidates", "Number of tags", 10,0,10 ));

    ////// Probe ///////
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_TrackPt = TH1F_LW::create( "m_Probe_TrackPt", "Probe Track Pt [MeV]", 100,0.,100000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_TrackEta = TH1F_LW::create( "m_Probe_TrackEta", "Probe Track Eta [MeV]", 54, -2.7, 2.7 ));
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_AbsCaloIso = TH1F_LW::create( "m_Probe_AbsCaloIso", "Probe - Abs Calo Isolation (E_{T}) [MeV]", 100,0.,50000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_RelCaloIso = TH1F_LW::create( "m_Probe_RelCaloIso", "Probe - Rel Calo Isolation (E_{T}/p_{T})", 100,0.,5. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_AbsJetIso = TH1F_LW::create( "m_Probe_AbsJetIso", "Probe - Abs Jet Isolation (p^{Jet}_{T}) [MeV]", 100,0.,50000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_RelJetIso = TH1F_LW::create( "m_Probe_RelJetIso", "Probe - Rel Jet Isolation (p^{Jet}_{T}/p_{T})", 100,0.,5. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_AbsTrkIso = TH1F_LW::create( "m_Probe_AbsTrkIso", "Probe - Abs Track Isolation (#Sigma p_{T} in cone) [MeV]", 100,0.,50000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_RelTrkIso = TH1F_LW::create( "m_Probe_RelTrkIso", "Probe - Rel Track Isolation (#Sigma p_{T} in cone/p_{T}) ", 100,0.,5.));
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_RelTrkIso_vs_pT = TProfile_LW::create("m_Probe_RelTrkIso_vs_pT","Relative Track Isolation vs ProbeTrack pT",50,0.,100.,0.,20.));
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_NCandidates = TH1F_LW::create( "m_Probe_NCandidates", "Number of probes", 10,0,10 ));

    ////// Tag and Probe pair ///////// 
    sc=muonphysmon_tagandprobecut.regHist( m_TagAndProbe_Cut_Flow = TH1F_LW::create( "m_TagAndProbe_Cut_Flow" , "TagAndProbe_Cut flow (1:#mu+trk, 2:Mass, 3:Charge, 4:#Delta #phi)", 6, 0.5, 6.5));
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ZMass = TH1F_LW::create( "m_TagAndProbe_ZMass", "TagAndProbe Z Invariant Mass [MeV]", 100,91200.-m_cut_diMuonMass,91200.+m_cut_diMuonMass ));
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_diMuonMass = TH1F_LW::create( "m_TagAndProbe_diMuonMass", "Invariant mass [MeV]", 100,0.,120000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_Pt = TH1F_LW::create( "m_TagAndProbe_Pt", "Invariant p_{T} of ID track pairs", 100,0.,100000 ));
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_DeltaPhi = TH1F_LW::create( "m_TagAndProbe_DeltaPhi", "#Delta#phi of ID track pairs", 64,0.,3.2 ));
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_DeltaR = TH1F_LW::create( "m_TagAndProbe_DeltaR", "#DeltaR of Tag&Probe pairs", 64, 0.,4. ));
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_NCandidates = TH1F_LW::create( "m_TagAndProbe_NCandidates", "Number of tag&probe pair candidates", 10,0,10 ));
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_Charge = TH1F_LW::create("m_TagAndProbe_Charge", "TagAndProbe Charge_{1}*Charge_{2}", 3,-1.5,1.5));
    sc=muonphysmon_tagandprobecut.regHist(m_Z_NCandidates = TH1F_LW::create( "m_Z_NCandidates", "Number of Z candidates", 10,0,10 ));

    //// Reconstruction efficiencies /////////
    MonGroup muonphysmon_efficiencies( this, m_muonphysicsmonitoring_path+"Efficiencies/TrackMuon", run, ATTRIB_UNMANAGED );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Efficiency_Eta = new TH1F("m_TagAndProbe_Efficiency_Eta" ,"TagAndProbe_Efficiency_Eta" , m_binsEta.size()-1 , &m_binsEta[0] ));
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Efficiency_Phi = new TH1F("m_TagAndProbe_Efficiency_Phi" ,"TagAndProbe_Efficiency_Phi" , m_binsPhi.size()-1 , &m_binsPhi[0] ));
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Efficiency_Pt = new TH1F("m_TagAndProbe_Efficiency_Pt" ,"TagAndProbe_Efficiency_Pt" , m_binsPt.size()-1 , &m_binsPt[0] ));

    //////// Probe track muon /////////////
    /*
      sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_Cut_Flow = TH1F_LW::create( "m_ProbeTrk_Cut_Flow" , "Probe Track - Cut Flow (1:nProbes, 2:p_{T}, 3:TrackIso, 4:nhSCT, 5:nhPIX, 6:d_{0}, 7:#chi^{2}/ndf)" , 8,0.5,8.5));
      sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_TrackPt = TH1F_LW::create( "m_ProbeTrk_TrackPt", "Probe Track - Track p_{T} [MeV]", 100,0,100000));
      sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_TrackIso=TH1F_LW::create("m_ProbeTrk_TrackIso","Probe Track - Relative Track Isolation (#Sigma p_{T} in cone/p_{T})", 100, 0,5));
      sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_NhitsSCT=TH1F_LW::create("m_ProbeTrk_NhitsSCT", "Probe Track - Number of SCT hits", 20,0,20));
      sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_NhitsPIX=TH1F_LW::create("m_ProbeTrk_NhitsPIX", "Probe Track - Number of PIX hits", 20,0,20));
      sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_d0=TH1F_LW::create("m_ProbeTrk_d0", "Probe Track - Impact Parameter d_{0} [mm]", 100, -5, 5));
      sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_chi2ndf=TH1F_LW::create("m_ProbeTrk_chi2ndf", "Probe Track - #chi^{2}/ndf", 80, 0, 20));
    */

    //Efficiencies truth
    MonGroup muonphysmon_efficiencies_truth( this, m_muonphysicsmonitoring_path+"Efficiencies/MuonReconstruction/Truth", run, ATTRIB_UNMANAGED );

    if (m_doTruth) {
      sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Eta_Truth = new TH1F( "m_TagAndProbe_Efficiency_Eta_Truth" ,"TagAndProbe_Efficiency_Eta_Truth" , m_binsEta.size()-1 , &m_binsEta[0] ));
      sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Phi_Truth = new TH1F( "m_TagAndProbe_Efficiency_Phi_Truth" ,"TagAndProbe_Efficiency_Phi_Truth" , m_binsPhi.size()-1 , &m_binsPhi[0] ));
      sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Pt_Truth = new TH1F( "m_TagAndProbe_Efficiency_Pt_Truth" ,"TagAndProbe_Efficiency_Pt_Truth" , m_binsPt.size()-1 , &m_binsPt[0] ));

      m_trk_pttruth    = TH1F_LW::create("m_trk_pttruth", "",m_binsPt.size()-1, &m_binsPt[0] );
      m_trk_etatruth   = TH1F_LW::create("m_trk_etatruth", "",m_binsEta.size()-1, &m_binsEta[0] );
      m_trk_phitruth   = TH1F_LW::create("m_trk_phitruth", "",m_binsPhi.size()-1, &m_binsPhi[0] );
    }

    if(sc.isFailure()) { 
      ATH_MSG_WARNING( "Failed to register histogram(s)" );   
      return StatusCode::FAILURE;
    }
    
    m_trk_pttot    = TH1F_LW::create("m_trk_pttot", "",m_binsPt.size()-1, &m_binsPt[0] );
    m_trk_etatot   = TH1F_LW::create("m_trk_etatot", "",m_binsEta.size()-1, &m_binsEta[0] );
    m_trk_phitot   = TH1F_LW::create("m_trk_phitot", "",m_binsPhi.size()-1, &m_binsPhi[0] );
    m_trk_ptpassed  = TH1F_LW::create("m_trk_ptpassed", "",m_binsPt.size()-1, &m_binsPt[0] );
    m_trk_etapassed = TH1F_LW::create("m_trk_etapassed", "",m_binsEta.size()-1, &m_binsEta[0] );
    m_trk_phipassed = TH1F_LW::create("m_trk_phipassed", "",m_binsPhi.size()-1, &m_binsPhi[0] );

    /*
      m_trk_pttrig = TH1F_LW::create("m_trk_pttrig", "",m_binsPt.size()-1, &m_binsPt[0] );
      m_trk_etatrig = TH1F_LW::create("m_trk_etatrig", "",m_binsEta.size()-1, &m_binsEta[0] );
      m_trk_phitrig = TH1F_LW::create("m_trk_phitrig", "",m_binsPhi.size()-1, &m_binsPhi[0] );
    */

    // Trk Quality Cuts
    std::string m_muontrkmonitoring_path = "Muon/MuonPhysicsMonitoring/TrkQualityCuts/";
    MonGroup muonphysmon_trkquality( this, m_muontrkmonitoring_path, run, ATTRIB_UNMANAGED );
    sc=muonphysmon_trkquality.regHist(m_mutrk_BLayerHits = TH1F_LW::create("global_BLayerHits","No. of BLayer Hits on TrackP",5,0.,5.) );
    sc=muonphysmon_trkquality.regHist(m_mutrk_BLayerSharedHits = TH1F_LW::create("global_BLayerSharedHits","No. of BLayer shared hits on TrackP",5,0.,5.) );
    sc=muonphysmon_trkquality.regHist(m_mutrk_PixelHits = TH1F_LW::create("global_PixelHits","No. of Pixel Hits on TrackP",10,0.,10.) );
    sc=muonphysmon_trkquality.regHist(m_mutrk_PixelLayers = TH1F_LW::create("global_PixelLayers","No. of Pixel Layers on TrackP",10,0.,10.) );
    sc=muonphysmon_trkquality.regHist(m_mutrk_SiHits = TH1F_LW::create("global_SiHits","No. of Pixel+SCT Hits on TrackP",25,0.,25.) );
    sc=muonphysmon_trkquality.regHist(m_mutrk_TRTHits = TH1F_LW::create("global_TRTHits","No. of TRT Hits on TrackP",100,0.,100.) );
    sc=muonphysmon_trkquality.regHist(m_mutrk_d0 = TH1F_LW::create("global_d0","d_{0} (CLHEP::mm)",100,-5.,5.) );
    sc=muonphysmon_trkquality.regHist(m_mutrk_pT = TH1F_LW::create("InnerTrk_pT","p_{T} (CLHEP::GeV)",50, 0.,100.) ); 
    sc=muonphysmon_trkquality.regHist(m_mutrk_chi2 = TH1F_LW::create("mutrack_chi2",  "chi2/DoF of reconstructed track", 80,-0.,20.) );
    sc=muonphysmon_trkquality.regHist(m_mutrk_relIso = TH1F_LW::create("mutrk_relIso", "Relative Track Isolation of reconstructed track", 100,-0.,5.));
    sc=muonphysmon_trkquality.regHist(m_mutrk_relIso_vs_pT = TProfile_LW::create("m_mutrk_relIso_vs_pT","Relative Track Isolation vs Track pT",50,0.,100.,0.,20.));
    sc=muonphysmon_trkquality.regHist(m_ptcone20_vs_etcone20 = TH2F_LW::create("m_ptcone20_vs_etcone20","Relative Track Isolation vs Relative Calorimeter Isolation",50,0.,10.,50, 0.,10.));
         
  }//isNewRun

  return sc;
} //============end of bookHistograms()=======================================================
//////////////////////////////////////////////////////////////////////////////////////////////
StatusCode MuTrackInsituPerformance::fillHistograms()
{
  ATH_MSG_DEBUG( "in fillHistograms()" ); 
  /*
    if (m_doTrigger) {
    if( m_trigDecTool->isPassed("EF_mu10")  ) {
    ATH_MSG_DEBUG( "Event passed the full trigger chain");
    m_TagAndProbe_Cut_Flow -> Fill(1);
    }
    else {
    ATH_MSG_INFO( "Event not triggered... Skipping");
    return StatusCode::SUCCESS;
    }
    } else  m_TagAndProbe_Cut_Flow -> Fill(1);
  */
    
  const Analysis::MuonContainer* importedMuonContainer;
  StatusCode sc = m_storeGate->retrieve(importedMuonContainer, m_MuonContainer );
  if (sc.isFailure() || !importedMuonContainer) {
    ATH_MSG_WARNING( "No muon collection ( "  << m_MuonContainer.c_str() << " ) found in StoreGate" );
    //    importedMuonContainer=0;
    //    return sc;
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_DEBUG( "Retrieved "<< m_MuonContainer );
  ATH_MSG_DEBUG( "Found "<< importedMuonContainer->size() << " muons in container" );

  //============================
  // Selection of tag muons:
  /* We select combined muons as good tag muons, because they have an inner detector track associated with a muon spectrometer track */
  //============================ 
  std::vector<const Rec::TrackParticle *> myTagMuons; 
  Analysis::MuonContainer::const_iterator muItr;

  // Loop on Staco/Muid collections, keep only combined (high-pt) muons, apply preselection cut on pt and eta
  for ( muItr=importedMuonContainer->begin(); muItr!=importedMuonContainer->end(); muItr++) {
    if( (*muItr)->isCombinedMuon() &&  (*muItr)->hasInDetTrackParticle() ) {
      double pt = (*muItr)->pt();
      if (pt==0) continue;
      const Rec::TrackParticle *mutrk = (*muItr)->inDetTrackParticle();
      double tagCaloIsolationET = (*muItr)->parameter(MuonParameters::etcone40);
      double tagJetIsolationPt = getJetEnergyInCone(mutrk);
      double tagTrkIsolationPt = (*muItr)->parameter(MuonParameters::ptcone40);
      double tagTrkIsolationPt_rel = tagTrkIsolationPt/pt;

      m_Tag_TrackEta->Fill((*muItr)->eta());
      m_Tag_TrackPt->Fill(pt);
      m_Tag_AbsCaloIso->Fill(tagCaloIsolationET);
      m_Tag_RelCaloIso->Fill(tagCaloIsolationET/pt);
      m_Tag_AbsJetIso->Fill(tagJetIsolationPt);
      m_Tag_RelJetIso->Fill(tagJetIsolationPt/pt);
      m_Tag_AbsTrkIso->Fill(tagTrkIsolationPt);
      m_Tag_RelTrkIso->Fill(tagTrkIsolationPt_rel);
      
      if ((*muItr)->pt() > m_cut_tagPt) 
	if ((*muItr)->eta() < m_cut_muonEta && m_cut_muonEta>0)
	  if (tagCaloIsolationET < m_cut_isolationET && m_cut_isolationET>0) 
	    if (tagJetIsolationPt < m_cut_isolationJet && m_cut_isolationJet>0)
	      if ( tagTrkIsolationPt_rel < m_cut_isolationTrk && m_cut_isolationTrk>0)
		if (!isElectron(mutrk)) {
		  myTagMuons.push_back((*muItr)->inDetTrackParticle());
		}
    }
  } // Loop on Staco/Muid collections
  
  m_Tag_NCandidates->Fill(myTagMuons.size());
  if (myTagMuons.size()==0) {
    ATH_MSG_DEBUG( "Can't find good tag muons. Skipping event..." );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Got "<< myTagMuons.size() << " tag muons after preselection" );

  //------------------------------------------------------------------------------
  //=====================================================
  // Selection of probe muons from inner detector tracks
  //=====================================================
  // Get Inner Detector tracks from StoreGate  
  const Rec::TrackParticleContainer *importedIDTrackContainer = 0;
  sc = m_storeGate->retrieve(importedIDTrackContainer, m_InnerTrackContainer);  
  if( sc.isFailure() || importedIDTrackContainer == 0) {
    ATH_MSG_WARNING( "No ID Track Particle Collection found in Storegate.  " <<  m_InnerTrackContainer << "Skipping event..." );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Retrieved "<< m_InnerTrackContainer );
  ATH_MSG_DEBUG( "Found "<< importedIDTrackContainer->size() << " ID tracks in container" );
  if ( importedIDTrackContainer->size()==0 ) { 
    ATH_MSG_DEBUG( "Empty container, skipping event" );
    return  StatusCode::SUCCESS;
  }
  
  std::vector<const Rec::TrackParticle *> myProbeTracks;
 
  /// iterators over the container 
  Rec::TrackParticleContainer::const_iterator trkItr  = importedIDTrackContainer->begin();
  Rec::TrackParticleContainer::const_iterator trkItrE = importedIDTrackContainer->end();

  for (; trkItr != trkItrE; ++trkItr) { 
    if (isPassedTrackQualityCuts((*trkItr))) {
      if (!isElectron((*trkItr))) {
	if (m_doProbeTrkMuonId && isAssociatedWithMSTrack((*trkItr))) {
	  double probeJetIsolationPt = getJetEnergyInCone((*trkItr));
	  double pt=(*trkItr)->pt();
	  m_Probe_TrackEta->Fill((*trkItr)->eta());
	  m_Probe_TrackPt->Fill(pt);
	  m_Probe_AbsJetIso->Fill(probeJetIsolationPt);
	  m_Probe_RelJetIso->Fill(probeJetIsolationPt/pt);
	  // m_Probe_AbsTrkIso->Fill(trkIso);
	  // m_Probe_RelTrkIso->Fill(trkIso_rel); 
	  myProbeTracks.push_back((*trkItr));
	}
      }
    } // isPassedTrackQualityCuts
  } // TrackParticleContainer
  
  m_Probe_NCandidates->Fill(myProbeTracks.size());
  if (myProbeTracks.size()==0) {
    ATH_MSG_DEBUG( "No probe tracks found, skipping event..." );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Got " << myProbeTracks.size() << " probes after track quality cuts" );
  
  //make tag&probe pairs-------------------------------
  std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > myTagsAndProbes;
  for (unsigned int i=0; i<myTagMuons.size(); i++) {
    const Rec::TrackParticle *tag = myTagMuons[i];
    for (unsigned int j=0; j<myProbeTracks.size(); j++) {
      const Rec::TrackParticle *probe = myProbeTracks[j];    
      if (tag==probe) continue;
      myTagsAndProbes.push_back(std::make_pair(tag,probe));
    }
  }
  m_TagAndProbe_NCandidates->Fill(myTagsAndProbes.size());

  std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > myZmuons;
  int nZ=0;
  for (unsigned int i=0; i<myTagsAndProbes.size(); i++) {
    const Rec::TrackParticle *tag = myTagsAndProbes[i].first;
    const Rec::TrackParticle *probe = myTagsAndProbes[i].second;
    //all probe muons
    m_trk_etatot->Fill(probe->eta(), 1.);
    m_trk_pttot->Fill(probe->pt(), 1.);
    m_trk_phitot->Fill(probe->phi(), 1.);

    /*
    // Tag - MuonProbe Matching 
    ATH_MSG_DEBUG(" > Fill dR matching.. " ); 
    double dR = m_analysisTools->deltaR(tag,probe);
    // Tag - muonProbe angle
    ATH_MSG_DEBUG(" > Fill angle.. "); 
    double dEta = tag->eta() - probe->eta();
    double dPhi = fabs(tag->phi() - probe->phi());
    if ( dPhi > twoPi ) dPhi = twoPi - dPhi;
    float dist = sqrt(dEta*dEta+dPhi*dPhi);
    ATH_MSG_DEBUG(" > tagProbeDistance.. " << dist); 
    //---cut: DeltaR cut
    ATH_MSG_DEBUG(" > DelatR tag&probe cut " ); 
    bool dRPassed = dR >= m_DRTagProbeMatchCut;
    if( !dRPassed ) {
    ATH_MSG_DEBUG("dR Cut failed");
    continue;
    }
    */

    //apply event selection-----------------------------------      
    if (isPassedZSelectionCuts(tag,probe,myTagsAndProbes)) { 
      ATH_MSG_DEBUG( "Probe track passed Z selection cuts");
      nZ++; 	
      myZmuons.push_back(std::make_pair(tag,probe));
      //passing probes
      m_trk_etapassed->Fill(probe->eta() , 1.);
      m_trk_ptpassed->Fill(probe->pt() , 1.);
      m_trk_phipassed->Fill(probe->phi(), 1.);   
      m_TagAndProbe_Cut_Flow->Fill(5);
      /*
	if (m_doTruth && isTrueZ[i]) {
	m_trk_etatruth->Fill(probe->eta(), 1.);
	m_trk_pttruth->Fill(probe->pt(), 1.);
	m_trk_phitruth->Fill(probe->phi(), 1.);
	}
      */
    }
    else ATH_MSG_DEBUG( "Probe track did not pass the Z selection cuts"); //failing probes
  }//close for
  
  if (myZmuons.size()==0) {
    ATH_MSG_DEBUG( "No Z candidates" );
    return StatusCode::SUCCESS;
  }  
  ATH_MSG_INFO( "Found " << nZ << " Z candidates" );
  nZReco +=nZ;
  m_Z_NCandidates->Fill(nZ);
  
  //*** matching of Z muons to truth ****************
  /*
    std::vector<bool> isTrueZ;
    if (m_doTruth) {
    for (unsigned int i=0; i<myZMuons.size(); i++) {
    std::vector<const Rec::TrackParticle *> muons;
    muons.push_back(myZMuons[i].first);
    muons.push_back(myZMuons[i].second);
    if ( myBPhysToolBox.isIdenticalCollection(muons,muonsTruth) ) {
    ATH_MSG_DEBUG( "Z matched with truth" );
    nZMatch++;
    isTrueZ.push_back(true);
    }
    else isTrueZ.push_back(false);
    }
    }
  */
  //*****************************************************
  
  return StatusCode::SUCCESS;
} 
//=============end of fillHistograms()======================================================
//////////////////////////////////////////////////////////////////////////////////////////////
StatusCode MuTrackInsituPerformance::procHistograms()
{
  ATH_MSG_DEBUG( "in procHistograms()" );

  if(endOfEventsBlock){}
  if(endOfLumiBlock){}
  if(endOfRun){

    // m_mutrk_relIso_vs_pT->LabelsDeflate("X");
    //if (m_mutrk_relIso_vs_pT->GetEntries()>0) m_mutrk_relIso_vs_pT->LabelsOption("va");   

    // m_Probe_RelTrkIso_vs_pT->LabelsDeflate("X");
    //if (m_Probe_RelTrkIso_vs_pT->GetEntries()>0) m_Probe_RelTrkIso_vs_pT->LabelsOption("va");   

    for (unsigned int j=1; j<=m_binsEta.size(); j++)
      if (m_trk_etatot->GetBinContent(j)>0) {
	m_TagAndProbe_Efficiency_Eta->Fill(m_binsEta[j-1], m_trk_etapassed->GetBinContent(j) / m_trk_etatot->GetBinContent(j));
      }
    m_TagAndProbe_Efficiency_Eta->SetMarkerStyle(2);
    m_TagAndProbe_Efficiency_Eta->SetOption("P");
    m_TagAndProbe_Efficiency_Eta->GetYaxis()->SetRangeUser(0.,1.1);
      
    for (unsigned int j=1; j<=m_binsPhi.size(); j++)
      if (m_trk_phitot->GetBinContent(j)>0) {
	m_TagAndProbe_Efficiency_Phi->Fill(m_binsPhi[j-1], m_trk_phipassed->GetBinContent(j) / m_trk_phitot->GetBinContent(j));
      }
    m_TagAndProbe_Efficiency_Phi->SetMarkerStyle(2);
    m_TagAndProbe_Efficiency_Phi->SetOption("P");
    m_TagAndProbe_Efficiency_Phi->GetYaxis()->SetRangeUser(0.,1.1);

    for (unsigned int j=1; j<=m_binsPt.size(); j++) 
      if (m_trk_pttot->GetBinContent(j)>0) {
	m_TagAndProbe_Efficiency_Pt->Fill(m_binsPt[j-1], m_trk_ptpassed->GetBinContent(j) / m_trk_pttot->GetBinContent(j));
      }
    m_TagAndProbe_Efficiency_Pt->SetMarkerStyle(2);
    m_TagAndProbe_Efficiency_Pt->SetOption("P");
    m_TagAndProbe_Efficiency_Pt->GetYaxis()->SetRangeUser(0.,1.1);

    if (m_doTruth) {
      for (unsigned int j=1; j<=m_binsEta.size(); j++)
	if (m_trk_etatot->GetBinContent(j)>0) {
	  m_TagAndProbe_Efficiency_Eta_Truth->Fill(m_binsEta[j-1], m_trk_etatruth->GetBinContent(j) / m_trk_etatot->GetBinContent(j));
	}
      m_TagAndProbe_Efficiency_Eta_Truth->SetMarkerStyle(3);
      m_TagAndProbe_Efficiency_Eta_Truth->SetMarkerColor(kRed);
      m_TagAndProbe_Efficiency_Eta_Truth->SetOption("P");
      m_TagAndProbe_Efficiency_Eta_Truth->GetYaxis()->SetRangeUser(0.,1.1);
      
      for (unsigned int j=1; j<=m_binsPhi.size(); j++)
	if (m_trk_phitot->GetBinContent(j)>0) {
	  m_TagAndProbe_Efficiency_Phi_Truth->Fill(m_binsPhi[j-1], m_trk_phitruth->GetBinContent(j) / m_trk_phitot->GetBinContent(j));
	}
      m_TagAndProbe_Efficiency_Phi_Truth->SetMarkerStyle(3);
      m_TagAndProbe_Efficiency_Phi_Truth->SetMarkerColor(kRed);
      m_TagAndProbe_Efficiency_Phi_Truth->SetOption("P");
      m_TagAndProbe_Efficiency_Phi_Truth->GetYaxis()->SetRangeUser(0.,1.1);

      for (unsigned int j=1; j<=m_binsPt.size(); j++) 
	if (m_trk_pttot->GetBinContent(j)>0) {
	  m_TagAndProbe_Efficiency_Pt_Truth->Fill(m_binsPt[j-1], m_trk_pttruth->GetBinContent(j) / m_trk_pttot->GetBinContent(j));
	}
      m_TagAndProbe_Efficiency_Pt_Truth->SetMarkerStyle(3);
      m_TagAndProbe_Efficiency_Pt_Truth->SetMarkerColor(kRed);
      m_TagAndProbe_Efficiency_Pt_Truth->SetOption("P");
      m_TagAndProbe_Efficiency_Pt_Truth->GetYaxis()->SetRangeUser(0.,1.1);
    }

    //de-allocate histograms
    LWHist::safeDelete(m_trk_pttot);
    LWHist::safeDelete(m_trk_phitot);
    LWHist::safeDelete(m_trk_etatot);
    LWHist::safeDelete(m_trk_ptpassed);
    LWHist::safeDelete(m_trk_phipassed);
    LWHist::safeDelete(m_trk_etapassed);
   
    /*
      m_trk_pttrig->Delete();
      m_trk_etatrig->Delete();
      m_trk_phitrig->Delete();
      m_trk_pttrig=0;
      m_trk_etatrig=0;
      m_trk_phitrig=0;
    */
    //  if (m_doTrigger) m_trigEffJpsiTools->finalize();
    
    /*
      if (m_doTruth) {
      std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<std::endl;
      std::cout<<"@ ---------------------------- Results @" <<std::endl;
      std::cout<<"@ nEvents:    " << nEvents <<std::endl;
      std::cout<<"@ nJpsiReco:  " << nJpsiReco <<std::endl;
      std::cout<<"@ nJpsiMatch: " << nJpsiMatch <<std::endl;
      std::cout<<"@ nJpsiTruth: " << nJpsiTruth <<std::endl;
      std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<std::endl;
      }
    */

  }  // isEndOfRun

  return StatusCode::SUCCESS;
} 
//=============end of procHistograms()==================================
////////////////////////////////////////////////////////////////////////
//======================================================================
StatusCode MuTrackInsituPerformance::finalize()
{    
  return StatusCode::SUCCESS;
}
//---------------------------------------------------------
bool MuTrackInsituPerformance::isElectron(const Rec::TrackParticle *track)const
{  
    
  if(!evtStore()->contains<ElectronContainer>(m_ElectronContainer) ) return -1;
  // retrieve the Electron container
  const ElectronContainer *elecTES = 0;
  if( evtStore()->retrieve(elecTES,m_ElectronContainer).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve electron container with key "+m_ElectronContainer);
    return false;
  }  
 
  ElectronContainer::const_iterator elecItr   = elecTES->begin();
  ElectronContainer::const_iterator elecItrE  = elecTES->end();

  const Rec::TrackParticle *elecTrack = 0;
  for(; elecItr != elecItrE; elecItr++) {
    elecTrack = (*elecItr)->trackParticle();
    if( elecTrack && (*elecItr)->isem(egammaPID::ElectronMedium) == 0 ) {
      if( track == elecTrack ) {
	return true;
      }
    }
  }
  return false;
} 

double MuTrackInsituPerformance::getJetEnergyInCone(const Rec::TrackParticle *track) const 
{
  ATH_MSG_DEBUG( "in getJetEnergyInCone()");

  if(!evtStore()->contains<JetCollection>(m_JetContainer) ) return -1;

  const JetCollection* mcpartJet=0; 
  if( evtStore()->retrieve(mcpartJet,m_JetContainer).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve Jet container with key "+m_JetContainer);
    return -1;
  }  

  JetCollection::const_iterator jetItr  = mcpartJet->begin();
  JetCollection::const_iterator jetItrE = mcpartJet->end();
  
  double minDeltaR = m_isoConeMax;
  double associatedJetPt = 0;
  for (; jetItr != jetItrE; ++jetItr) {
    double dR = sqrt( pow(track->eta() - (*jetItr)->eta(),2) + pow(track->phi() - (*jetItr)->phi(),2) );
    if ( dR<minDeltaR) {
      minDeltaR = dR;
      associatedJetPt = (*jetItr)->pt();
    }
  }
  return associatedJetPt;
}  

double MuTrackInsituPerformance::getImpactParameter(const Rec::TrackParticle *track)const
{
  ATH_MSG_DEBUG( "in getImpactParameter()" );

  // Get primary vertex from StoreGate
  const VxContainer* importedVxContainer=0; //Points to primary vertices
 
  // protection against cosmics for VxPrimary
  if (evtStore()->contains<VxContainer>(m_vxContainerName)) {
    if (evtStore()->retrieve(importedVxContainer,m_vxContainerName).isFailure() ) {
      ATH_MSG_DEBUG ("Could not retrieve primary vertex container with key "+m_vxContainerName);
      return -1;
    }
  } else {
    ATH_MSG_DEBUG ("StoreGate doesn't contain primary vertex container with key "+m_vxContainerName);
    return -1;
  } 
 
  const Trk::VxCandidate *myVxPrimaryCandidate = *(importedVxContainer->begin());
  if (!myVxPrimaryCandidate) {
    ATH_MSG_WARNING( "No Primary Vertex found in container, cannot estimate d0" );
    return -1; 
  }
  ATH_MSG_DEBUG( "Retrieving impact parameter with TrackToVertexIPEstimator" );
  const Trk::ImpactParametersAndSigma *myIPS = m_trackToVertexIPEstimator->estimate( track, myVxPrimaryCandidate, true);
  if (!myIPS) {
    ATH_MSG_INFO("d0 calculation failed");
    return -1;
  }
  double d0wrtPvx = myIPS->IPd0;
  //  double d0ErrwrtPvx = myIPS->sigmad0;
  ATH_MSG_DEBUG( "Deleting ImpactParametersAndSigma object" );
  ////delete myIPS;
  
  return d0wrtPvx;
}

///////////////////////////////////////////////////
//IsolP(TrackParticle)
///////////////////////////////////////////////////
double MuTrackInsituPerformance::getTrackIsolationP(const INavigable4Momentum *particle)
{
  ATH_MSG_DEBUG( "in getTrackIsolationP()" );

  const Analysis::Muon *muon = dynamic_cast<const Analysis::Muon*> (particle);
  const Rec::TrackParticle *trackParticle = 0; 
  if ( muon ) {
    //---1 cut: muon type
    if( m_doMuonTypeCut ){
      ATH_MSG_DEBUG(" > Muon type cut " ); 
      if ( muon->isCombinedMuon() && m_muonType == "combined"){
	trackParticle = muon->track();
      } else if (muon->isStandAloneMuon() && m_muonType == "standalone"){
	trackParticle = muon->track();
      } else if ( muon->hasInDetTrackParticle() && m_muonType == "tagged"){
	trackParticle = muon->track();
      } else {
	ATH_MSG_DEBUG( "type cut failed" );
	return 1e99;
      }    
    }
    else trackParticle = muon->track();
  }
  else trackParticle = dynamic_cast<const Rec::TrackParticle*>(particle);
  
  if ( !trackParticle ) {
    ATH_MSG_WARNING( "No track particle associated to the trackParticle candidate" );
    return -1;
  }
 
  // Pt, Eta, Phi
  //double pt = trackParticle->pt();
  //double eta = trackParticle->eta();
  //double phi = trackParticle->phi();
  
  // isolation
  //bool * selfIsRemoved = NULL;
  double IsolP = 1.; //m_isolationTool->trackIsolationEnergy(trackParticle, m_trk_coneIso);
  return IsolP;
}

bool MuTrackInsituPerformance::isPassedZSelectionCuts(const Rec::TrackParticle *track1,const Rec::TrackParticle *track2, std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > /*listOfTrackPairs*/ )
{
  ATH_MSG_DEBUG( "in isPassedZSelectionCuts()" );
  
  //double probePtMax=0;
  //int    probePtMaxId=0;

  double charge1 = track1->charge();
  //double pt1 = track1->pt();
  double phi1 = track1->phi();
  double eta1 = track1->eta(); 
     
  double charge2 = track2->charge();
  //double pt2 = track2->pt();
  double phi2 = track2->phi();  
  double eta2 = track2->eta(); 

  double pt = ( track1->hlv() + track2->hlv() ).perp();
  double deltaPhi=fabs(phi1-phi2);
  if (deltaPhi>3.14159) deltaPhi=6.28318-deltaPhi; 
  double invMass = ( track1->hlv() + track2->hlv() ).m(); 
  double charge = charge1*charge2;

  // track1 --> tag
  // track2 --> probe

  // Tag - MuonProbe Matching 
  ATH_MSG_DEBUG(" > Fill dR matching.. " ); 
  double dR = m_analysisTools->deltaR(track1,track2);

  //Keep quantities before cuts      
  m_TagAndProbe_DeltaR->Fill(dR);

  // Tag - muonProbe angle
  ATH_MSG_DEBUG(" > Fill angle.. "); 
  double dEta = eta1 - eta2;
  double dPhi = fabs(phi1 - phi2);
  if ( dPhi > twoPi ) dPhi = twoPi - dPhi;
  float dist = sqrt(dEta*dEta+dPhi*dPhi);
  ATH_MSG_DEBUG(" > tagProbeDistance.. " << dist); 
  //---cut: DeltaR cut
  ATH_MSG_DEBUG(" > DelatR tag&probe cut " ); 
  bool dRPassed = dR >= m_DRTagProbeMatchCut;

  if( !dRPassed ) {
    ATH_MSG_DEBUG("dR Cut failed");
    return false;
  }

  //Keep quantities before cuts      
  m_TagAndProbe_Charge->Fill(charge);
  m_TagAndProbe_diMuonMass->Fill(invMass);
  m_TagAndProbe_DeltaPhi->Fill(deltaPhi);
  m_TagAndProbe_Pt->Fill(pt);
  //    m_TagAndProbe_PtMax->Fill(pt1);
  //    m_TagAndProbe_PtMin->Fill(pt2);
  
  //fill histograms for muon pairs, before cuts
  m_TagAndProbe_Cut_Flow->Fill(1);
  
  //apply cuts
  ATH_MSG_DEBUG( "Applying cuts..." );
  if (charge==m_cut_charge || m_cut_charge==0) {
    m_TagAndProbe_Cut_Flow->Fill(2);
    if ( fabs(invMass-91200.) < m_cut_diMuonMass || m_cut_diMuonMass==0) {
      m_TagAndProbe_Cut_Flow->Fill(3);
      if (deltaPhi>m_cut_deltaPhi || m_cut_deltaPhi==0) {
	m_TagAndProbe_Cut_Flow->Fill(4);	 
	ATH_MSG_DEBUG( "Got a Z candidate" );
	m_TagAndProbe_ZMass->Fill(invMass);
	//	if (pt2>probePtMax) { 
	//	  probePtMax=pt2;
	//	  probePtMaxId=myZmuons.size()-1;	
	return true;
      }
    }
  }
  
  return false;
}

bool MuTrackInsituPerformance::isAssociatedWithMSTrack(const Rec::TrackParticle *trk) 
{  
  ATH_MSG_DEBUG( "in isAssociatedWithMStrack()" );

  const Analysis::MuonContainer* importedMuonContainer;
  if(StatusCode sc = m_storeGate->retrieve(importedMuonContainer, m_MuonContainer ).isFailure()){
    ATH_MSG_WARNING( "Couldn't retrieve MuonContainer from StoreGate" );
    return StatusCode::SUCCESS;
  }
  
  Analysis::MuonContainer::const_iterator muItr;
  for ( muItr=importedMuonContainer->begin(); muItr!=importedMuonContainer->end(); muItr++) {
    if ((*muItr)->hasInDetTrackParticle()) {
      //      if ( (*muItr)->combinedMuonTrackParticle() == trk ) {
      if ( (*muItr)->inDetTrackParticle()==trk) {
	return true;
      }
    }
    else if ( fabs( (*muItr)->eta()-trk->eta())<10e-3 && fabs( (*muItr)->phi()-trk->phi())<10e-3 )
      return true;
  }
  return false;
}
///////////////////////////////////////////////////////////////////////////////
// applyEnergyIsolation
///////////////////////////////////////////////////////////////////////////////

bool MuTrackInsituPerformance::getCaloEnergyIsolation(const Analysis::Muon* muonCandidate) {
/*
  double eIso = m_muonIsolationTool->isolationEnergy(muonCandidate, m_energyIsoCone);
  double eIsoRatio = -9999.;
  double pt = muonCandidate->pt();
  double eta= muonCandidate->eta();
  if (eIso>0) {
    if (pt>0) {
      eIsoRatio  = log10(eIso/pt);
    }
  }
 
  double eIsoCut = 0;
  double ratioCutEt = 0;
  if( fabs(eta)<1.5 ) {
    eIsoCut     =   m_eIsoBarrelCut;
    ratioCutEt  =   m_eIsoPtRatioBarrelCut;
  }
  else if (fabs(eta) > 1.5 && fabs(eta) < 1.8) {
    eIsoCut     =   m_eIsoTransitionCut;
    ratioCutEt  =   m_eIsoPtRatioTransitionCut;
  }
  else {
    eIsoCut     =   m_eIsoEndCapCut;
    ratioCutEt  =   m_eIsoPtRatioEndCapCut;
  }  
     
  ATH_MSG_DEBUG("Isolation Energy in " << m_energyIsoCone << " cone is: " << eIso << ", eta: " << eta << ",  pt: " << pt );
  ATH_MSG_DEBUG("Et Iso: " << eIso << " Et Iso over pt: " << eIsoRatio);
  
  if( eIso < eIsoCut && eIsoRatio < ratioCutEt ) {
    ATH_MSG_DEBUG("EtIso and EtIso over pt cut passed.");
    return true;
  }
  else 
    return false;
*/
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// getTrackParameters
///////////////////////////////////////////////////////////////////////////////
const Trk::TrackParameters* MuTrackInsituPerformance::getTrkParams(const Trk::Track* trk) {
  // --- Retrieve the last measured hit which is closest to the solenoid/calorimeter entrance --- 
  const DataVector<const Trk::TrackParameters>* paramvec = trk->trackParameters();
  const Trk::TrackParameters* param = 0;
  if (paramvec) { 
    DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();
    param = *(--itEnd);
  }
  return param;
}

///////////////////////////////////////////////////////////////////////////////
// applyTrackIsolation 
///////////////////////////////////////////////////////////////////////////////
bool MuTrackInsituPerformance::applyTrkIsol(const Rec::TrackParticle* ptcl) {
/*
  double ptIso = m_muonIsolationTool->trackIsolationEnergy(ptcl, m_trk_coneIso);
  double ptIsoRatio = -9999;
  if (ptIso>0) {
    double pt = ptcl->pt();
    if (pt>0) // ---> just for safety (pt can never be 0 actually)
      ptIsoRatio = log10(ptIso/pt);
  }  
  ATH_MSG_DEBUG("ptIso over pt: " << ptIsoRatio );
  if( ptIso==0 || ptIsoRatio < m_trk_relTrkIso ) {
    return true;
  }
  else {
    ATH_MSG_INFO("Did not pass track isolation cut.");
    return false;
  }
*/
    return true;
}
  
bool MuTrackInsituPerformance::isPassedTrackQualityCuts(const Rec::TrackParticle *trk)
{
  ATH_MSG_DEBUG( "in isPassedTrackQualityCuts()" );
 
  ATH_MSG_DEBUG( "Retrieving TrackSummary Information" );
  const Trk::TrackSummary* trkSummary(0);
  trkSummary = trk->trackSummary();
  if (trkSummary==0) {
    ATH_MSG_INFO( "Could not create TrackSummary - Track will likely fail hits requirements" );  
    return false;
  }

  int nhitsSCT = trkSummary->get(Trk::numberOfSCTHits);
  m_mutrk_SiHits->Fill((float) nhitsSCT);
  int nhitsPixel = trkSummary->get(Trk::numberOfPixelHits); //including the b-layer    
  m_mutrk_PixelHits->Fill((float) nhitsPixel);
   
  m_mutrk_BLayerHits->Fill((float) trkSummary->get(Trk::numberOfBLayerHits));
  m_mutrk_BLayerSharedHits->Fill((float) trkSummary->get(Trk::numberOfBLayerSharedHits));
  m_mutrk_PixelLayers->Fill((float) trkSummary->get(Trk::numberOfContribPixelLayers));
  m_mutrk_TRTHits->Fill((float) trkSummary->get(Trk::numberOfTRTHits));
  //          int nSiHits = trkSummary->get(Trk::numberOfPixelHits) + trkSummary->get(Trk::numberOfSCTHits);
  //          m_mutrk_SiHits->Fill((float) nSiHits);


  double ndf = trk->fitQuality()->doubleNumberDoF();
  
  if (ndf<=0){
    ATH_MSG_INFO( "Skiping event while encountering track with " << ndf << " degrees of freedom" );
    return false;
  }
  
  double chi2ndf = trk->fitQuality()->chiSquared() / ndf;
  m_mutrk_chi2->Fill(chi2ndf); 
 
  double d0 = getImpactParameter(trk);
  m_mutrk_d0->Fill(d0);

  double pt=trk->pt();
  // ---> just for safety (pt can never be 0 actually)
  if (pt<=0){
    ATH_MSG_INFO( "Skiping event while pt is negative " << pt  );
    return false;
  }

  ATH_MSG_DEBUG(" > Calculating ProbeTrack pT in GeV.. " << pt/1000.0 ); 
  m_mutrk_pT->Fill(pt/1000.);
  
  ////////////////////////////////////////////////////////////////////////////////////
  bool selfIsRemoved = false;
  ATH_MSG_DEBUG(" > Calculating Track isolation.. " ); 
  
 // double trkIso = m_isolationTool->trackIsolationEnergy(trk, m_trk_coneIso, true , &selfIsRemoved );
 // ATH_MSG_DEBUG("Track Isolation Momentum (CLHEP::GeV) in " << m_trk_coneIso << "cone is  " << trkIso/1000.0 ); 

 // double trkIso20 = m_isolationTool->trackIsolationEnergy(trk, 0.2, true , &selfIsRemoved );
 // ATH_MSG_DEBUG("Track Isolation Momentum (CLHEP::GeV) in cone 0.2 is  " << trkIso20/1000.0 ); 

  if(m_CaloIsol)
    {
      ATH_MSG_DEBUG(" > Calculating Calo isolation.. " ); 

     // double eIso = m_isolationTool->summedCellEnergy(trk, m_energyIsoCone);
     // double eIso20 = m_isolationTool->summedCellEnergy(trk, 0.2);
     // ATH_MSG_DEBUG("Isolation Energy in " << m_energyIsoCone << " cone is: " << eIso/1000.); 
     // ATH_MSG_DEBUG("Isolation Energy in cone 0.2 is:  " << eIso20/1000.); 
     // m_ptcone20_vs_etcone20->Fill(eIso20/pt, trkIso20/pt);
    }
   double trkIso = 0;
  if (trkIso <= 0) return false;  
  double trkIso_rel = -9999;
  
  ////////////////////////////////////////////////////////////////////////////////////  
 
  //trkIso_rel = log10(trkIso/pt);
 // trkIso_rel = trkIso/pt;
 // m_mutrk_relIso->Fill(trkIso_rel);
 // ATH_MSG_DEBUG("ptIsolation over pt: " << trkIso_rel );
  //m_mutrk_relIso_vs_pT->Fill(pt/1000., trkIso_rel);
 
  if ( pt > m_trk_pt )
    //    if (trkIso_rel < m_trk_relTrkIso || m_trk_relTrkIso==0)
   // if ( trkIso_rel < m_trk_relTrkIso )
      if ( nhitsSCT >= m_trk_nhitsSCT )
	if ( nhitsPixel >= m_trk_nhitsPix )
	  //	  if (d0 < m_trk_d0 || m_trk_d0==0)
	  if ( d0 < m_trk_d0 )
	    //             if (chi2ndf < m_trk_chi2ndf || m_trk_chi2ndf==0) {       
	    if ( chi2ndf < m_trk_chi2ndf ){       
	      double probeJetIsolationPt = getJetEnergyInCone(trk);
	      m_Probe_TrackEta->Fill(trk->eta());
	      m_Probe_TrackPt->Fill(pt);
	      m_Probe_AbsJetIso->Fill(probeJetIsolationPt);
	      m_Probe_RelJetIso->Fill(probeJetIsolationPt/pt);
	     // m_Probe_AbsTrkIso->Fill(trkIso);
	    //  m_Probe_RelTrkIso->Fill(trkIso/pt);
	     // m_Probe_RelTrkIso_vs_pT->Fill(pt/1000., trkIso/pt);	                   
	      return true;
	    }	 
  return false;
}
