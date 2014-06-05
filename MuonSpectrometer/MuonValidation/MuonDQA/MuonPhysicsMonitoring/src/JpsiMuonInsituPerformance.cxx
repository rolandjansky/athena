/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   N. Benekos(Illinois) - I. Nomidis (Thessaloniki)
// MuonDQA
// Mar. 2009
//
// DESCRIPTION:
// Subject: 	This class does a full Insitu-Performance of the MuonSpectrometer
//		reconstruction efficiency, Inner-Tracker-Reconstruction Efficiency and
//		Trigger Efficiency
//		It is derivated from MuonPhysicsAnalysisMonitoring
///////////////////////////////////////////////////////////////////////////

#include "MuonPhysicsMonitoring/JpsiMuonInsituPerformance.h"

JpsiMuonInsituPerformance::JpsiMuonInsituPerformance( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_iVertexFitter("Trk::TrkVKalVrtFitter"),
   m_helpertool("InDet::ConversionFinderUtils"),
   m_vertexEstimator("InDet::VertexPointEstimator"),
   m_V0Tools("Trk::V0Tools"),          
   m_trigEffJpsiTools("TrigEffJpsiTools")
{
  declareProperty( "doMCTruth",          	m_doTruth);
  declareProperty( "InnerTrackContainer" ,      m_InnerTrackContainer="TrackParticleCandidate" );
  declareProperty( "MuonCollection",            m_muonCollectionName ); //the muon container to calculate efficiency
  declareProperty( "MuonContainer",	        m_MuonContainer );//the tag muon container (combined muons)
  declareProperty( "TruthParticleContainer",    m_TruthParticleContainer);
  declareProperty( "TrackParticleTruthCollection", m_TrackParticleTruthCollection );
  declareProperty( "McEventCollection",         m_McEventCollection );

  declareProperty( "deltaRMinCut",	        m_cut_deltaRMin =0.);
  declareProperty( "deltaRMaxCut",	        m_cut_deltaRMax=0.);
  declareProperty( "diMuonMassWindow",	        m_cut_diMuonMass = 0);
  declareProperty( "tagCombinedMuonOnly",       m_tagCombinedMuonOnly=true);
  declareProperty( "tagMuonPtCut",              m_cut_tagMuonPt = 0);
  declareProperty( "probeMuonPtCut" ,           m_cut_probeMuonPt = 0); //preselection pt-cut on muons
  declareProperty( "muonEtaCut",                m_cut_muonEta=2.5); //preselection eta-cut on muons 
  declareProperty( "properTimeCut",             m_cut_properTime=0.); //pseudo proper-time cut to select only direct Jpsi
  declareProperty( "vertexChi2Cut",             m_cut_JpsiVertexChi2 = 0);

  declareProperty( "lowerMassCut",              m_cut_lowerMass = 0);
  declareProperty( "upperMassCut",              m_cut_upperMass = 0);

  declareProperty( "TRTHitsCut",                m_cut_nhTRT = 0);
  declareProperty( "TRTOutliersCut",            m_cut_noTRT = 0);
  declareProperty( "PixHitsCut",                m_cut_nhPix = 0);
  declareProperty( "SCTHitsCut",                m_cut_nhSCT = 0);
  declareProperty( "d0SignificanceCut",         m_cut_d0Significance = 0);
  declareProperty( "chi2Cut",                   m_cut_chi2 = 0);
  declareProperty( "pCut",                      m_cut_p = 0);


  declareProperty( "jpsiMass",                  m_jpsiMass = 0);
  declareProperty( "jpsiMassBinning",           m_jpsiMassBinning=0. );
  declareProperty( "jpsiSigma",                 m_jpsiSigma=0. );
  declareProperty( "jpsiSideband",              m_jpsiSideband=10. , "size in sigma");
  declareProperty( "jpsiSignalRegion",          m_jpsiSignalRegion=3., "size in sigma");

  declareProperty( "binsEta",                   m_binsEta );
  declareProperty( "binsPhi",                   m_binsPhi );
  declareProperty( "binsPt",                    m_binsPt );

  // Vertexing
  declareProperty( "vertexCut",                 m_doVertexing );
  declareProperty( "TrkVertexFitterTool",       m_iVertexFitter);
  declareProperty( "V0Tools",                   m_V0Tools);
  declareProperty( "ConversionFinderHelperTool",m_helpertool);
  declareProperty( "VertexPointEstimator",      m_vertexEstimator);

  // Trigger
  declareProperty( "useTrigger",                m_doTrigger);
  declareProperty( "TrigEffJpsiTools", 		m_trigEffJpsiTools);
  declareProperty( "tagMatchL1",                m_tagMatchL1);
  declareProperty( "tagMatchL2",                m_tagMatchL2);
  declareProperty( "tagMatchEF",                m_tagMatchEF);
  declareProperty( "TriggerSelection",          m_trigSelection );

  declareProperty( "doTriggerEffL1",            m_doTriggerEffL1);
  declareProperty( "doTriggerEffL2",            m_doTriggerEffL2);
  declareProperty( "doTriggerEffEF",            m_doTriggerEffEF);
  declareProperty( "TriggerChains",             m_trigMenus );

  declareProperty( "doEfficiencies",            m_doEfficiencies );
  declareProperty( "doBkgExtraction",           m_doBkgExtraction);
  declareProperty( "doBkgExtractionAfterProbeAssociation", m_doBkgExtractionAfterProbeAssociation);
  
}

//======================================================================================//
/**  Initialize */
//======================================================================================//
StatusCode JpsiMuonInsituPerformance:: initialize() 
{ 
  StatusCode sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  ATH_MSG_DEBUG( "initialize()" );
  /** Initialize tools and services */
  sc = service("StoreGateSvc", m_storeGate);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR( "Unable to retrieve pointer to StoreGateSvc" );
    return StatusCode::FAILURE;
  }

  /// get a handle on the analysis tools
  IToolSvc* toolSvc;
  sc = service("ToolSvc",toolSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR( " Can't get ToolSvc" );
    return StatusCode::FAILURE;
  }
  IAlgTool *tmp_analysisTools;
  sc = toolSvc->retrieveTool("AnalysisTools",tmp_analysisTools);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR( "Can't get handle on analysis tools" );
    return StatusCode::FAILURE;
  }
  m_analysisTools=dynamic_cast<IAnalysisTools *>(tmp_analysisTools);

  sc = service("THistSvc", m_thistSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR( "Unable to retrieve pointer to THistSvc" );
    return StatusCode::FAILURE;
  }


  // Vertexing tools
  
  // Get the VKalVrt vertex fitting tool from ToolSvc
  if ( m_iVertexFitter.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve " << m_iVertexFitter );
    return StatusCode::FAILURE;
  }     
  // retrieving the V0 tools
  if ( m_V0Tools.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve tool " << m_V0Tools );
    return StatusCode::FAILURE;
  }
  // Get the vertex point estimator tool from ToolSvc
  if ( m_vertexEstimator.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve tool " << m_vertexEstimator );
    return StatusCode::FAILURE;
  }
  // Get the helpertool from ToolSvc
  if ( m_helpertool.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve tool " << m_helpertool );
    return StatusCode::FAILURE;
  }

  if ( m_doTrigger ) {
    //retrieve trigger matching tool
    sc = m_trigEffJpsiTools.retrieve();
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "Could not retrieve TrigEffJpsiTool" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( "Got handle on TrigEffJpsiTools" );
      sc = m_trigEffJpsiTools->initialize();
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "Could not initialize TrigEffJpsiTool" );
	return StatusCode::FAILURE;
      }
    }
  }  
  
  nEvents = 0;
  nJpsiReco = 0;  
  nJpsiMatch = 0;  
  nJpsiTruth = 0;  

  return StatusCode::SUCCESS; 
}

//=============================================================================================================
StatusCode JpsiMuonInsituPerformance::bookHistogramsRecurrent()
{
  ATH_MSG_DEBUG( "JpsiMuonInsituPerformance::in bookHistograms()" );
  
  if(newEventsBlock){}
  if(newLumiBlock){}
  if(newRun) {  

    StatusCode sc = StatusCode::SUCCESS;
    //////////////////////////////////////////////////////////////////
    std::string m_muonphysicsmonitoring_path = "Muon/MuonPhysicsMonitoring/Jpsimumu/";
    
    MonGroup muonphysmon_tagandprobecut( this, m_muonphysicsmonitoring_path+"TagAndProbe_Cut_Histograms", run, ATTRIB_UNMANAGED );
    
    int nBins = (m_cut_upperMass - m_cut_lowerMass)/m_jpsiMassBinning;

    m_EventSelection_Cut_Flow = new TH1F("m_EventSelection_Cut_Flow","Event selection Cut Flow 0:All/GRL, 1:Trig, 2:PV, 3:CollisionCand, 4:Muons",6,-0.5,5.5);
    sc=muonphysmon_tagandprobecut.regHist(m_EventSelection_Cut_Flow);

    m_Tag_Cut_Flow = new TH1F( "m_Tag_Cut_Flow" , "Tag cut flow (0:AllMuons, 1:type, 2:p_{T}, 3:#eta, 4:trkQuality, 5:L1, 6:L2, 7:EF )" , 10,-0.5,9.5);
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_Cut_Flow);

    m_Probe_Nhits_vs_NoutliersTRT = new TH2F("m_Probe_Nhits_vs_NoutliersTRT","Probe #hits vs #outliers in TRT",50,0,50,50,0,50);
    m_Probe_NhitsSCT = new TH1F("m_Probe_NhitsSCT","Probe #hits in SCT",50,0,50);
    m_Probe_NhitsPix = new TH1F("m_Probe_NhitsPix","Probe #hits in Pixel",50,0,50);
    m_Probe_d0Sig = new TH1F("m_Probe_d0Sig","Probe d_{0} significance",50,0,50);
    m_Probe_chi2ndf = new TH1F("m_Probe_chi2ndf","Probe #chi^{2}/ndf",100,0,100);
    m_Probe_P = new TH1F("m_Probe_P","Probe p",100,0,20000);
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_Nhits_vs_NoutliersTRT);
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_NhitsSCT);
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_NhitsPix);
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_d0Sig);
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_chi2ndf);
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_P);

    //if (m_doTrigger) m_TagAndProbe_Cut_Flow = new TH1F( "m_TagAndProbe_Cut_Flow" , "TagAndProbe_Cut flow (0:All, 1:#mu+trk, 2:Vrtx, 3:#chi^{2}, 4:#Delta R, 5:#tau, 6:!electron, 7:Mass, 8:ProbeAssocMS, 9:ProbeAssocL1Trig, 10:ProbeAssocL2Trig)" , 12,-0.5,11.5);
    m_TagAndProbe_Cut_Flow = new TH1F( "m_TagAndProbe_Cut_Flow" , "TagAndProbe_Cut flow (1:#mu+trk, 2:Vrtx, 3:#chi^{2}, 4:#Delta R, 5:#tau, 6:!electron, 7:Mass, 8:ProbeAssociation )" , 9,0.5,9.5);
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_Cut_Flow);

    m_TagAndProbe_ID_diMuonMass = new TH1F( "m_TagAndProbe_ID_diMuonMass", "Di-muon invariant mass [MeV]", 125,0.,10000. );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_diMuonMass);

    m_TagAndProbe_ID_diMuonMass_AfterVtx = new TH1F( "m_TagAndProbe_ID_diMuonMass_AfterVtx", "Di-muon invariant mass after vertexing [MeV]", 125,0.,10000. );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_diMuonMass_AfterVtx);

    m_TagAndProbe_ID_diMuonMass_Refitted = new TH1F( "m_TagAndProbe_ID_diMuonMass_Refitted", "Refitted di-muon invariant mass [MeV]", 125,0.,10000. );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_diMuonMass_Refitted);

    m_TagAndProbe_ID_TrackPt = new TH1F( "m_TagAndProbe_ID_TrackPt", "Track Pt [MeV]", 80,0.,80000. );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_TrackPt);

    m_TagAndProbe_ID_NCandidates = new TH1F( "m_TagAndProbe_ID_NCandidates", "TagAndProbe Number of J/#psi Candidate Pairs of ID tracks", 10,0,10 );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_NCandidates);

    m_TagAndProbe_ID_Selected_RefJpsiMass = new TH1F( "m_TagAndProbe_ID_Selected_RefJpsiMass", "Selected J/#psi refitted mass [MeV]", nBins , m_cut_lowerMass, m_cut_upperMass );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_Selected_RefJpsiMass);

    m_TagAndProbe_InvariantMass = new TH1F( "m_TagAndProbe_InvariantMass", "T&P invariant mass (#mu+trk) [MeV]",nBins , m_cut_lowerMass, m_cut_upperMass );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_InvariantMass);

    m_TagAndProbe_InvariantMass_AfterAssociationSTACO = new TH1F( "m_TagAndProbe_InvariantMass_AfterAssociationSTACO", "T&P invariant mass (#mu+#mu) [MeV]",nBins , m_cut_lowerMass, m_cut_upperMass );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_InvariantMass_AfterAssociationSTACO);

    m_TagAndProbe_InvariantMass_AfterAssociationMUID = new TH1F( "m_TagAndProbe_InvariantMass_AfterAssociationMUID", "T&P invariant mass (#mu+#mu) [MeV]",nBins , m_cut_lowerMass, m_cut_upperMass );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_InvariantMass_AfterAssociationMUID);

    m_TagAndProbe_InvariantMass_SameSign = new TH1F( "m_TagAndProbe_InvariantMass_SameSign", "T&P invariant mass (#mu+trk) Same Sign [MeV]",nBins , m_cut_lowerMass, m_cut_upperMass );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_InvariantMass_SameSign);

    m_TagAndProbe_InvariantMass_SameSign_AfterAssociationSTACO = new TH1F( "m_TagAndProbe_InvariantMass_SameSign_AfterAssociationSTACO", "T&P invariant mass (#mu+#mu) SameSign [MeV]",nBins , m_cut_lowerMass, m_cut_upperMass );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_InvariantMass_SameSign_AfterAssociationSTACO);

    m_TagAndProbe_InvariantMass_SameSign_AfterAssociationMUID = new TH1F( "m_TagAndProbe_InvariantMass_SameSign_AfterAssociationMUID", "T&P invariant mass (#mu+#mu) SameSign [MeV]",nBins , m_cut_lowerMass, m_cut_upperMass );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_InvariantMass_SameSign_AfterAssociationMUID);


    m_TagAndProbe_DeltaR = new TH1F( "m_TagAndProbe_DeltaR", "T&P #DeltaR", 50,0.,5. );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_DeltaR);

    m_TagAndProbe_Charge = new TH1F( "m_TagAndProbe_Charge", "T&P q_{1}*q_{2}", 3,-1.5,1.5);
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_Charge);

    m_TagAndProbe_Propertime = new TH1F( "m_TagAndProbe_Propertime", "T&P pseudo-propertime #tau [ps]", 100,-2.5,2.5 );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_Propertime);

    m_TagAndProbe_VertexChi2 = new TH1F( "m_TagAndProbe_VertexChi2","T&P vertex #chi^{2}/ndf",100,0,200);
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_VertexChi2);

    MonGroup muonphysmon_efficiencies( this, m_muonphysicsmonitoring_path+"Efficiencies/", run, ATTRIB_UNMANAGED );

    //Mass
    m_TagAndProbe_Mass_BeforeProbeAssociation = new TH1F("m_TagAndProbe_Mass_BeforeProbeAssociation","TagAndProbe_Mass_BeforeProbeAssociation",nBins,m_cut_lowerMass,m_cut_upperMass);
    m_TagAndProbe_Mass_BeforeProbeAssociation_L = new TH1F("m_TagAndProbe_Mass_BeforeProbeAssociation_L","TagAndProbe_Mass_BeforeProbeAssociation_L",nBins,m_cut_lowerMass,m_cut_upperMass);
    m_TagAndProbe_Mass_BeforeProbeAssociation_R = new TH1F("m_TagAndProbe_Mass_BeforeProbeAssociation_R","TagAndProbe_Mass_BeforeProbeAssociation_R",nBins,m_cut_lowerMass,m_cut_upperMass) ;
    m_TagAndProbe_Mass_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_Mass_BeforeProbeAssociation_SR","TagAndProbe_Mass_BeforeProbeAssociation_SR",nBins,m_cut_lowerMass,m_cut_upperMass);
    m_TagAndProbe_MassBkg_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_MassBkg_BeforeProbeAssociation_SR","m_TagAndProbe_MassBkg_BeforeProbeAssociation_SR",nBins,m_cut_lowerMass,m_cut_upperMass);
    m_TagAndProbe_MassBkgExtracted_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_MassBkgExtracted_BeforeProbeAssociation_SR","m_TagAndProbe_MassBkgExtracted_BeforeProbeAssociation_SR",nBins,m_cut_lowerMass,m_cut_upperMass);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Mass_BeforeProbeAssociation);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Mass_BeforeProbeAssociation_L);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Mass_BeforeProbeAssociation_R);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Mass_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_MassBkg_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_MassBkgExtracted_BeforeProbeAssociation_SR);

    //Pt
    m_TagAndProbe_Pt_BeforeProbeAssociation_L = new TH1F("m_TagAndProbe_Pt_BeforeProbeAssociation_L","TagAndProbe_Pt_BeforeProbeAssociation_L",m_binsPt.size()-1, &m_binsPt[0] );
    m_TagAndProbe_Pt_BeforeProbeAssociation_R = new TH1F("m_TagAndProbe_Pt_BeforeProbeAssociation_R","TagAndProbe_Pt_BeforeProbeAssociation_R",m_binsPt.size()-1, &m_binsPt[0] );
    m_TagAndProbe_Pt_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_Pt_BeforeProbeAssociation_SR","TagAndProbe_Pt_BeforeProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] );
    m_TagAndProbe_PtBkg_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_PtBkg_BeforeProbeAssociation_SR","TagAndProbe_PtBkg_BeforeProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] );
    m_TagAndProbe_PtBkgExtracted_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_PtBkgExtracted_BeforeProbeAssociation_SR","TagAndProbe_PtBkgExtracted_BeforeProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Pt_BeforeProbeAssociation_L);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Pt_BeforeProbeAssociation_R);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Pt_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_PtBkg_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_PtBkgExtracted_BeforeProbeAssociation_SR);
    
    //Phi
    m_TagAndProbe_Phi_BeforeProbeAssociation_L = new TH1F("m_TagAndProbe_Phi_BeforeProbeAssociation_L","TagAndProbe_Phi_BeforeProbeAssociation_L",m_binsPhi.size()-1, &m_binsPhi[0] );
    m_TagAndProbe_Phi_BeforeProbeAssociation_R = new TH1F("m_TagAndProbe_Phi_BeforeProbeAssociation_R","TagAndProbe_Phi_BeforeProbeAssociation_R",m_binsPhi.size()-1, &m_binsPhi[0] );
    m_TagAndProbe_Phi_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_Phi_BeforeProbeAssociation_SR","TagAndProbe_Phi_BeforeProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] );
    m_TagAndProbe_PhiBkg_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_PhiBkg_BeforeProbeAssociation_SR","TagAndProbe_PhiBkg_BeforeProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] );
    m_TagAndProbe_PhiBkgExtracted_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_PhiBkgExtracted_BeforeProbeAssociation_SR","TagAndProbe_PhiBkgExtracted_BeforeProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Phi_BeforeProbeAssociation_L);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Phi_BeforeProbeAssociation_R);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Phi_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_PhiBkg_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_PhiBkgExtracted_BeforeProbeAssociation_SR);

    //Eta
    m_TagAndProbe_Eta_BeforeProbeAssociation_L = new TH1F("m_TagAndProbe_Eta_BeforeProbeAssociation_L","TagAndProbe_Eta_BeforeProbeAssociation_L",m_binsEta.size()-1, &m_binsEta[0] );
    m_TagAndProbe_Eta_BeforeProbeAssociation_R = new TH1F("m_TagAndProbe_Eta_BeforeProbeAssociation_R","TagAndProbe_Eta_BeforeProbeAssociation_R",m_binsEta.size()-1, &m_binsEta[0] );
    m_TagAndProbe_Eta_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_Eta_BeforeProbeAssociation_SR","TagAndProbe_Eta_BeforeProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] );
    m_TagAndProbe_EtaBkg_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_EtaBkg_BeforeProbeAssociation_SR","TagAndProbe_EtaBkg_BeforeProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] );
    m_TagAndProbe_EtaBkgExtracted_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_EtaBkgExtracted_BeforeProbeAssociation_SR","TagAndProbe_EtaBkgExtracted_BeforeProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Eta_BeforeProbeAssociation_L);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Eta_BeforeProbeAssociation_R);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Eta_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_EtaBkg_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_EtaBkgExtracted_BeforeProbeAssociation_SR);

    ///////////////////////////////////    Same sign

    //Mass
    m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation = new TH1F("m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation","TagAndProbe_Mass_SameSign_BeforeProbeAssociation",nBins,m_cut_lowerMass,m_cut_upperMass);
    m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_L = new TH1F("m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_L","TagAndProbe_Mass_SameSign_BeforeProbeAssociation_L",nBins,m_cut_lowerMass,m_cut_upperMass);
    m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_R = new TH1F("m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_R","TagAndProbe_Mass_SameSign_BeforeProbeAssociation_R",nBins,m_cut_lowerMass,m_cut_upperMass) ;
    m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_SR","TagAndProbe_Mass_SameSign_BeforeProbeAssociation_SR",nBins,m_cut_lowerMass,m_cut_upperMass);
    m_TagAndProbe_MassBkg_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_MassBkg_SameSign_BeforeProbeAssociation_SR","m_TagAndProbe_MassBkg_SameSign_BeforeProbeAssociation_SR",nBins,m_cut_lowerMass,m_cut_upperMass);
    m_TagAndProbe_MassBkgExtracted_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_MassBkgExtracted_SameSign_BeforeProbeAssociation_SR","m_TagAndProbe_MassBkgExtracted_SameSign_BeforeProbeAssociation_SR",nBins,m_cut_lowerMass,m_cut_upperMass);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_L);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_R);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_MassBkg_SameSign_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_MassBkgExtracted_SameSign_BeforeProbeAssociation_SR);

    //Pt
    m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_L = new TH1F("m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_L","TagAndProbe_Pt_SameSign_BeforeProbeAssociation_L",m_binsPt.size()-1, &m_binsPt[0] );
    m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_R = new TH1F("m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_R","TagAndProbe_Pt_SameSign_BeforeProbeAssociation_R",m_binsPt.size()-1, &m_binsPt[0] );
    m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_SR","TagAndProbe_Pt_SameSign_BeforeProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] );
    m_TagAndProbe_PtBkg_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_PtBkg_SameSign_BeforeProbeAssociation_SR","TagAndProbe_PtBkg_SameSign_BeforeProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] );
    m_TagAndProbe_PtBkgExtracted_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_PtBkgExtracted_SameSign_BeforeProbeAssociation_SR","TagAndProbe_PtBkgExtracted_SameSign_BeforeProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_L);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_R);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_PtBkg_SameSign_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_PtBkgExtracted_SameSign_BeforeProbeAssociation_SR);
    
    //Phi
    m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_L = new TH1F("m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_L","TagAndProbe_Phi_SameSign_BeforeProbeAssociation_L",m_binsPhi.size()-1, &m_binsPhi[0] );
    m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_R = new TH1F("m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_R","TagAndProbe_Phi_SameSign_BeforeProbeAssociation_R",m_binsPhi.size()-1, &m_binsPhi[0] );
    m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_SR","TagAndProbe_Phi_SameSign_BeforeProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] );
    m_TagAndProbe_PhiBkg_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_PhiBkg_SameSign_BeforeProbeAssociation_SR","TagAndProbe_PhiBkg_SameSign_BeforeProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] );
    m_TagAndProbe_PhiBkgExtracted_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_PhiBkgExtracted_SameSign_BeforeProbeAssociation_SR","TagAndProbe_PhiBkgExtracted_SameSign_BeforeProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_L);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_R);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_PhiBkg_SameSign_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_PhiBkgExtracted_SameSign_BeforeProbeAssociation_SR);

    //Eta
    m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_L = new TH1F("m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_L","TagAndProbe_Eta_SameSign_BeforeProbeAssociation_L",m_binsEta.size()-1, &m_binsEta[0] );
    m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_R = new TH1F("m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_R","TagAndProbe_Eta_SameSign_BeforeProbeAssociation_R",m_binsEta.size()-1, &m_binsEta[0] );
    m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_SR","TagAndProbe_Eta_SameSign_BeforeProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] );
    m_TagAndProbe_EtaBkg_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_EtaBkg_SameSign_BeforeProbeAssociation_SR","TagAndProbe_EtaBkg_SameSign_BeforeProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] );
    m_TagAndProbe_EtaBkgExtracted_SameSign_BeforeProbeAssociation_SR = new TH1F("m_TagAndProbe_EtaBkgExtracted_SameSign_BeforeProbeAssociation_SR","TagAndProbe_EtaBkgExtracted_SameSign_BeforeProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_L);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_R);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_EtaBkg_SameSign_BeforeProbeAssociation_SR);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_EtaBkgExtracted_SameSign_BeforeProbeAssociation_SR);


    //efficiency: total probes
    m_TagAndProbe_Ntotal_vs_Pt = new TH1F("m_TagAndProbe_Ntotal_vs_Pt", "",m_binsPt.size()-1, &m_binsPt[0] );
    m_TagAndProbe_Ntotal_vs_Eta= new TH1F("m_TagAndProbe_Ntotal_vs_Eta", "",m_binsEta.size()-1, &m_binsEta[0] );
    m_TagAndProbe_Ntotal_vs_Phi= new TH1F("m_TagAndProbe_Ntotal_vs_Phi", "",m_binsPhi.size()-1, &m_binsPhi[0] );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Ntotal_vs_Pt);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Ntotal_vs_Eta);
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Ntotal_vs_Phi);
      

    //Reconstruction efficiencies
    for (unsigned int j=0; j<m_muonCollectionName.size(); j++) {
      MonGroup muonphysmon_recoefficiencies( this, m_muonphysicsmonitoring_path+"Efficiencies/MuonReconstruction/"+m_muonCollectionName[j], run, ATTRIB_UNMANAGED );

      m_TagAndProbe_Pt_AfterProbeAssociation_L.push_back( new TH1F("m_TagAndProbe_Pt_AfterProbeAssociation_L","TagAndProbe_Pt_AfterProbeAssociation_L",m_binsPt.size()-1, &m_binsPt[0] ));
      m_TagAndProbe_Pt_AfterProbeAssociation_R.push_back( new TH1F("m_TagAndProbe_Pt_AfterProbeAssociation_R","TagAndProbe_Pt_AfterProbeAssociation_R",m_binsPt.size()-1, &m_binsPt[0] ));
      m_TagAndProbe_Pt_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_Pt_AfterProbeAssociation_SR","TagAndProbe_Pt_AfterProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] ));
      m_TagAndProbe_PtBkg_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_PtBkg_AfterProbeAssociation_SR","TagAndProbe_PtBkg_AfterProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] ));
      m_TagAndProbe_PtBkgExtracted_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_PtBkgExtracted_AfterProbeAssociation_SR","TagAndProbe_PtBkgExtracted_AfterProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] ));
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Pt_AfterProbeAssociation_L.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Pt_AfterProbeAssociation_R.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Pt_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_PtBkg_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_PtBkgExtracted_AfterProbeAssociation_SR.back());


      m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_L.push_back( new TH1F("m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_L","TagAndProbe_Pt_SameSign_AfterProbeAssociation_L",m_binsPt.size()-1, &m_binsPt[0] ));
      m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_R.push_back( new TH1F("m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_R","TagAndProbe_Pt_SameSign_AfterProbeAssociation_R",m_binsPt.size()-1, &m_binsPt[0] ));
      m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_SR","TagAndProbe_Pt_SameSign_AfterProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] ));
      m_TagAndProbe_PtBkg_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_PtBkg_SameSign_AfterProbeAssociation_SR","TagAndProbe_PtBkg_SameSign_AfterProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] ));
      m_TagAndProbe_PtBkgExtracted_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_PtBkgExtracted_SameSign_AfterProbeAssociation_SR","TagAndProbe_PtBkgExtracted_SameSign_AfterProbeAssociation_SR",m_binsPt.size()-1, &m_binsPt[0] ));
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_L.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_R.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_PtBkg_SameSign_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_PtBkgExtracted_SameSign_AfterProbeAssociation_SR.back());

      m_TagAndProbe_Eta_AfterProbeAssociation_L.push_back( new TH1F("m_TagAndProbe_Eta_AfterProbeAssociation_L","TagAndProbe_Eta_AfterProbeAssociation_L",m_binsEta.size()-1, &m_binsEta[0] ));
      m_TagAndProbe_Eta_AfterProbeAssociation_R.push_back( new TH1F("m_TagAndProbe_Eta_AfterProbeAssociation_R","TagAndProbe_Eta_AfterProbeAssociation_R",m_binsEta.size()-1, &m_binsEta[0] ));
      m_TagAndProbe_Eta_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_Eta_AfterProbeAssociation_SR","TagAndProbe_Eta_AfterProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] ));
      m_TagAndProbe_EtaBkg_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_EtaBkg_AfterProbeAssociation_SR","TagAndProbe_EtaBkg_AfterProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] ));
      m_TagAndProbe_EtaBkgExtracted_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_EtaBkgExtracted_AfterProbeAssociation_SR","TagAndProbe_EtaBkgExtracted_AfterProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] ));
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Eta_AfterProbeAssociation_L.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Eta_AfterProbeAssociation_R.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Eta_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_EtaBkg_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_EtaBkgExtracted_AfterProbeAssociation_SR.back());


      m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_L.push_back( new TH1F("m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_L","TagAndProbe_Eta_SameSign_AfterProbeAssociation_L",m_binsEta.size()-1, &m_binsEta[0] ));
      m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_R.push_back( new TH1F("m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_R","TagAndProbe_Eta_SameSign_AfterProbeAssociation_R",m_binsEta.size()-1, &m_binsEta[0] ));
      m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_SR","TagAndProbe_Eta_SameSign_AfterProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] ));
      m_TagAndProbe_EtaBkg_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_EtaBkg_SameSign_AfterProbeAssociation_SR","TagAndProbe_EtaBkg_SameSign_AfterProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] ));
      m_TagAndProbe_EtaBkgExtracted_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_EtaBkgExtracted_SameSign_AfterProbeAssociation_SR","TagAndProbe_EtaBkgExtracted_SameSign_AfterProbeAssociation_SR",m_binsEta.size()-1, &m_binsEta[0] ));
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_L.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_R.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_EtaBkg_SameSign_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_EtaBkgExtracted_SameSign_AfterProbeAssociation_SR.back());



      m_TagAndProbe_Phi_AfterProbeAssociation_L.push_back( new TH1F("m_TagAndProbe_Phi_AfterProbeAssociation_L","TagAndProbe_Phi_AfterProbeAssociation_L",m_binsPhi.size()-1, &m_binsPhi[0] ));
      m_TagAndProbe_Phi_AfterProbeAssociation_R.push_back( new TH1F("m_TagAndProbe_Phi_AfterProbeAssociation_R","TagAndProbe_Phi_AfterProbeAssociation_R",m_binsPhi.size()-1, &m_binsPhi[0] ));
      m_TagAndProbe_Phi_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_Phi_AfterProbeAssociation_SR","TagAndProbe_Phi_AfterProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] ));
      m_TagAndProbe_PhiBkg_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_PhiBkg_AfterProbeAssociation_SR","TagAndProbe_PhiBkg_AfterProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] ));
      m_TagAndProbe_PhiBkgExtracted_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_PhiBkgExtracted_AfterProbeAssociation_SR","TagAndProbe_PhiBkgExtracted_AfterProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] ));
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Phi_AfterProbeAssociation_L.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Phi_AfterProbeAssociation_R.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Phi_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_PhiBkg_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_PhiBkgExtracted_AfterProbeAssociation_SR.back());


      m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_L.push_back( new TH1F("m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_L","TagAndProbe_Phi_SameSign_AfterProbeAssociation_L",m_binsPhi.size()-1, &m_binsPhi[0] ));
      m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_R.push_back( new TH1F("m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_R","TagAndProbe_Phi_SameSign_AfterProbeAssociation_R",m_binsPhi.size()-1, &m_binsPhi[0] ));
      m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_SR","TagAndProbe_Phi_SameSign_AfterProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] ));
      m_TagAndProbe_PhiBkg_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_PhiBkg_SameSign_AfterProbeAssociation_SR","TagAndProbe_PhiBkg_SameSign_AfterProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] ));
      m_TagAndProbe_PhiBkgExtracted_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_PhiBkgExtracted_SameSign_AfterProbeAssociation_SR","TagAndProbe_PhiBkgExtracted_SameSign_AfterProbeAssociation_SR",m_binsPhi.size()-1, &m_binsPhi[0] ));
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_L.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_R.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_PhiBkg_SameSign_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_PhiBkgExtracted_SameSign_AfterProbeAssociation_SR.back());
	
          
      //      std::string hMassAfterAssName = "m_TagAndProbe_Mass_AfterProbeAssociation"+m_muonCollectionName[j];
      m_TagAndProbe_Mass_AfterProbeAssociation.push_back( new TH1F("m_TagAndProbe_Mass_AfterProbeAssociation","TagAndProbe_Mass_AfterProbeAssociation",nBins,m_cut_lowerMass,m_cut_upperMass) );
      m_TagAndProbe_Mass_AfterProbeAssociation_L.push_back( new TH1F("m_TagAndProbe_Mass_AfterProbeAssociation_L","TagAndProbe_Mass_AfterProbeAssociation_L",nBins , m_cut_lowerMass, m_cut_upperMass) );
      m_TagAndProbe_Mass_AfterProbeAssociation_R.push_back( new TH1F("m_TagAndProbe_Mass_AfterProbeAssociation_R","TagAndProbe_Mass_AfterProbeAssociation_R",nBins , m_cut_lowerMass, m_cut_upperMass) );
      m_TagAndProbe_Mass_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_Mass_AfterProbeAssociation_SR","TagAndProbe_Mass_AfterProbeAssociation_SR",nBins , m_cut_lowerMass, m_cut_upperMass) );
      m_TagAndProbe_MassBkgExtracted_AfterProbeAssociation.push_back( new TH1F("m_TagAndProbe_MassBkgExtracted_AfterProbeAssociation","TagAndProbe_MassBkgExtracted_AfterProbeAssociation",nBins , m_cut_lowerMass, m_cut_upperMass) );
      m_TagAndProbe_MassBkgExtracted_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_MassBkgExtracted_AfterProbeAssociation_SR","TagAndProbe_MassBkgExtracted_AfterProbeAssociation_SR",nBins , m_cut_lowerMass, m_cut_upperMass) );

      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Mass_AfterProbeAssociation.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Mass_AfterProbeAssociation_L.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Mass_AfterProbeAssociation_R.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Mass_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_MassBkgExtracted_AfterProbeAssociation.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_MassBkgExtracted_AfterProbeAssociation_SR.back());

      m_TagAndProbe_Mass_SameSign_AfterProbeAssociation.push_back( new TH1F("m_TagAndProbe_Mass_SameSign_AfterProbeAssociation","TagAndProbe_Mass_SameSign_AfterProbeAssociation",nBins,m_cut_lowerMass,m_cut_upperMass) );
      m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_L.push_back( new TH1F("m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_L","TagAndProbe_Mass_SameSign_AfterProbeAssociation_L",nBins , m_cut_lowerMass, m_cut_upperMass) );
      m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_R.push_back( new TH1F("m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_R","TagAndProbe_Mass_SameSign_AfterProbeAssociation_R",nBins , m_cut_lowerMass, m_cut_upperMass) );
      m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_SR","TagAndProbe_Mass_SameSign_AfterProbeAssociation_SR",nBins , m_cut_lowerMass, m_cut_upperMass) );
      m_TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation.push_back( new TH1F("m_TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation_SR","TagAndProbe_MassBkgEctracted_SameSign_AfterProbeAssociation_SR",nBins , m_cut_lowerMass, m_cut_upperMass) );
      m_TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation_SR.push_back( new TH1F("m_TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation_SR","TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation_SR",nBins , m_cut_lowerMass, m_cut_upperMass) );

      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Mass_SameSign_AfterProbeAssociation.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_L.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_R.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_SR.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation_SR.back());
      
      //efficiency: passed probes
      m_TagAndProbe_NpassedReco_vs_Pt.push_back( new TH1F("m_TagAndProbe_NpassedReco_vs_Pt", "",m_binsPt.size()-1, &m_binsPt[0] ));
      m_TagAndProbe_NpassedReco_vs_Eta.push_back( new TH1F("m_TagAndProbe_NpassedReco_vs_Eta", "",m_binsEta.size()-1, &m_binsEta[0] ));
      m_TagAndProbe_NpassedReco_vs_Phi.push_back( new TH1F("m_TagAndProbe_NpassedReco_vs_Phi","",m_binsPhi.size()-1, &m_binsPhi[0] ));
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_NpassedReco_vs_Pt.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_NpassedReco_vs_Phi.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_NpassedReco_vs_Eta.back());

      //efficiency
      m_TagAndProbe_Efficiency_Eta.push_back( new TH1F( "m_TagAndProbe_Efficiency_Eta" ,"TagAndProbe_Efficiency_Eta" , m_binsEta.size()-1 , &m_binsEta[0] ));      
      m_TagAndProbe_Efficiency_Phi.push_back( new TH1F( "m_TagAndProbe_Efficiency_Phi" ,"TagAndProbe_Efficiency_Phi" , m_binsPhi.size()-1 , &m_binsPhi[0] ));
      m_TagAndProbe_Efficiency_Pt.push_back( new TH1F( "m_TagAndProbe_Efficiency_Pt" ,"TagAndProbe_Efficiency_Pt" , m_binsPt.size()-1 , &m_binsPt[0] ));
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Efficiency_Eta.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Efficiency_Phi.back());
      sc=muonphysmon_recoefficiencies.regHist(m_TagAndProbe_Efficiency_Pt.back());

    }

    if (m_doTrigger) {
      if (m_doTriggerEffL1) {
	//L1 trigger efficiencies
	MonGroup muonphysmon_L1MU4trigefficiencies( this, m_muonphysicsmonitoring_path+"Efficiencies/Trigger/L1_MU4", run, ATTRIB_UNMANAGED );
	MonGroup muonphysmon_L1MU6trigefficiencies( this, m_muonphysicsmonitoring_path+"Efficiencies/Trigger/L1_MU6", run, ATTRIB_UNMANAGED );
	
	if (m_doEfficiencies) {
	  
	  m_TagAndProbe_L1TriggerEfficiency_Eta.push_back( new TH1F( "m_TagAndProbe_L1_MU4_TrigEfficiency_Eta" ,"TagAndProbe_L1_MU4_TrigEfficiency_Eta" , m_binsEta.size()-1 , &m_binsEta[0] ));
	  sc=muonphysmon_L1MU4trigefficiencies.regHist(m_TagAndProbe_L1TriggerEfficiency_Eta.back());
	  m_TagAndProbe_L1TriggerEfficiency_Phi.push_back( new TH1F( "m_TagAndProbe_L1_MU4_TrigEfficiency_Phi" ,"TagAndProbe_L1_MU4_TrigEfficiency_Phi" , m_binsPhi.size()-1 , &m_binsPhi[0] ));
	  sc=muonphysmon_L1MU4trigefficiencies.regHist(m_TagAndProbe_L1TriggerEfficiency_Phi.back());
	  m_TagAndProbe_L1TriggerEfficiency_Pt.push_back( new TH1F( "m_TagAndProbe_L1_MU4_TrigEfficiency_Pt" ,"TagAndProbe_L1_MU4_TrigEfficiency_Pt" , m_binsPt.size()-1 , &m_binsPt[0] ));
	  sc=muonphysmon_L1MU4trigefficiencies.regHist(m_TagAndProbe_L1TriggerEfficiency_Pt.back());
	  
	  m_TagAndProbe_L1TriggerEfficiency_Eta.push_back( new TH1F( "m_TagAndProbe_L1_MU6_TrigEfficiency_Eta" ,"TagAndProbe_L1_MU6_TrigEfficiency_Eta" , m_binsEta.size()-1 , &m_binsEta[0] ));
	  sc=muonphysmon_L1MU6trigefficiencies.regHist(m_TagAndProbe_L1TriggerEfficiency_Eta.back());
	  m_TagAndProbe_L1TriggerEfficiency_Phi.push_back( new TH1F( "m_TagAndProbe_L1_MU6_TrigEfficiency_Phi" ,"TagAndProbe_L1_MU6_TrigEfficiency_Phi" , m_binsPhi.size()-1 , &m_binsPhi[0] ));
	  sc=muonphysmon_L1MU6trigefficiencies.regHist(m_TagAndProbe_L1TriggerEfficiency_Phi.back());
	  m_TagAndProbe_L1TriggerEfficiency_Pt.push_back( new TH1F( "m_TagAndProbe_L1_MU6_TrigEfficiency_Pt" ,"TagAndProbe_L1_MU6_TrigEfficiency_Pt" , m_binsPt.size()-1 , &m_binsPt[0] ));
	  sc=muonphysmon_L1MU6trigefficiencies.regHist(m_TagAndProbe_L1TriggerEfficiency_Pt.back());

	}
	//MU4
	m_TagAndProbe_NpassedL1_vs_Pt.push_back(  new TH1F("m_h_pttrigL1_MU4", "",m_binsPt.size()-1, &m_binsPt[0] ) );
	m_TagAndProbe_NpassedL1_vs_Eta.push_back( new TH1F("m_h_etatrigL1_MU4", "",m_binsEta.size()-1, &m_binsEta[0] ) );
	m_TagAndProbe_NpassedL1_vs_Phi.push_back( new TH1F("m_h_phitrigL1_MU4", "",m_binsPhi.size()-1, &m_binsPhi[0] ) );
	sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedL1_vs_Pt.back());
	sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedL1_vs_Eta.back());
	sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedL1_vs_Phi.back());
	//MU6
	m_TagAndProbe_NpassedL1_vs_Pt.push_back(  new TH1F("m_h_pttrigL1_MU6", "",m_binsPt.size()-1, &m_binsPt[0] ) );
	m_TagAndProbe_NpassedL1_vs_Eta.push_back( new TH1F("m_h_etatrigL1_MU6", "",m_binsEta.size()-1, &m_binsEta[0] ) );
	m_TagAndProbe_NpassedL1_vs_Phi.push_back( new TH1F("m_h_phitrigL1_MU6", "",m_binsPhi.size()-1, &m_binsPhi[0] ) );
	sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedL1_vs_Pt.back());
	sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedL1_vs_Eta.back());
	sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedL1_vs_Phi.back());	      
      }
    
      if (m_doTriggerEffL2) {
	//L2 trigger efficiencies
	for (unsigned int itrig=0; itrig<m_trigMenus.size(); itrig++) {
	  MonGroup muonphysmon_L2trigefficiencies( this, m_muonphysicsmonitoring_path+"Efficiencies/Trigger/"+m_trigMenus[itrig], run, ATTRIB_UNMANAGED );
	  if (m_doEfficiencies) {
	    
	    std::string htrigname;
	    htrigname = "m_TagAndProbe_"+m_trigMenus[itrig]+"_TrigEfficiency_Eta";
	    m_TagAndProbe_L2TriggerEfficiency_Eta.push_back( new TH1F( htrigname.c_str(), htrigname.c_str() , m_binsEta.size()-1 , &m_binsEta[0] ));
	    sc=muonphysmon_L2trigefficiencies.regHist(m_TagAndProbe_L2TriggerEfficiency_Eta.back());
	    htrigname = "m_TagAndProbe_"+m_trigMenus[itrig]+"_TrigEfficiency_Phi";
	    m_TagAndProbe_L2TriggerEfficiency_Phi.push_back( new TH1F( htrigname.c_str(), htrigname.c_str() , m_binsPhi.size()-1 , &m_binsPhi[0] ));
	    sc=muonphysmon_L2trigefficiencies.regHist(m_TagAndProbe_L2TriggerEfficiency_Phi.back());
	    htrigname = "m_TagAndProbe_"+m_trigMenus[itrig]+"_TrigEfficiency_Pt";
	    m_TagAndProbe_L2TriggerEfficiency_Pt.push_back( new TH1F( htrigname.c_str(), htrigname.c_str() , m_binsPt.size()-1 , &m_binsPt[0] ));
	    sc=muonphysmon_L2trigefficiencies.regHist(m_TagAndProbe_L2TriggerEfficiency_Pt.back());
	    
	  }
	  std::string htrigname;
	  htrigname = "m_h_pttrig"+m_trigMenus[itrig];
	  m_TagAndProbe_NpassedL2_vs_Pt.push_back(  new TH1F(htrigname.c_str(), "",m_binsPt.size()-1, &m_binsPt[0] ) );
	  htrigname = "m_h_etatrig"+m_trigMenus[itrig];
	  m_TagAndProbe_NpassedL2_vs_Eta.push_back( new TH1F(htrigname.c_str(), "",m_binsEta.size()-1, &m_binsEta[0] ) );
	  htrigname = "m_h_phitrig"+m_trigMenus[itrig];
	  m_TagAndProbe_NpassedL2_vs_Phi.push_back( new TH1F(htrigname.c_str(), "",m_binsPhi.size()-1, &m_binsPhi[0] ) );
	  sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedL2_vs_Pt.back());
	  sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedL2_vs_Eta.back());
	  sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedL2_vs_Phi.back());     
	}
      }
    }
    
    //Efficiencies truth
    if (m_doTruth) {
      MonGroup muonphysmon_efficiencies_truth( this, m_muonphysicsmonitoring_path+"Efficiencies/MuonReconstruction/Truth", run, ATTRIB_UNMANAGED );
      
      if (m_doEfficiencies) {
	m_TagAndProbe_Efficiency_Eta_Truth = new TH1F( "m_TagAndProbe_Efficiency_Eta_Truth" ,"TagAndProbe_Efficiency_Eta_Truth" , m_binsEta.size()-1 , &m_binsEta[0] );
	sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Eta_Truth);
	
	m_TagAndProbe_Efficiency_Phi_Truth = new TH1F( "m_TagAndProbe_Efficiency_Phi_Truth" ,"TagAndProbe_Efficiency_Phi_Truth" , m_binsPhi.size()-1 , &m_binsPhi[0] );
	sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Phi_Truth);
	
	m_TagAndProbe_Efficiency_Pt_Truth = new TH1F( "m_TagAndProbe_Efficiency_Pt_Truth" ,"TagAndProbe_Efficiency_Pt_Truth" , m_binsPt.size()-1 , &m_binsPt[0] );
	sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Pt_Truth);	
      }
      m_TagAndProbe_NpassedTruth_vs_Pt = new TH1F("m_h_pttruth", "",m_binsPt.size()-1, &m_binsPt[0] );
      m_TagAndProbe_NpassedTruth_vs_Eta= new TH1F("m_h_etatruth", "",m_binsEta.size()-1, &m_binsEta[0] );
      m_TagAndProbe_NpassedTruth_vs_Phi= new TH1F("m_h_phitruth", "",m_binsPhi.size()-1, &m_binsPhi[0] );
      sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedTruth_vs_Pt);
      sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedTruth_vs_Phi);
      sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_NpassedTruth_vs_Eta);
    } // do Truth
    
    
    if(sc.isFailure()) { 
      ATH_MSG_FATAL( "Failed to register histogram(s)" );   
      return sc;
    }
    
    
  }//isNewRun
  
  return StatusCode::SUCCESS;
} //==================== end of bookHistograms() ========================================================================

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode JpsiMuonInsituPerformance::fillHistograms()
{
  ATH_MSG_DEBUG( "in fillHistograms()" ); 
  nEvents++;
  m_EventSelection_Cut_Flow->Fill(0);

  //analyse only events which have passed a full trigger chain
  if (m_doTrigger && m_trigSelection!="") {
    if (m_trigDecTool->isPassed(m_trigSelection)==false) {
      ATH_MSG_INFO( "Event didn't pass trigger "<< m_trigSelection <<". Skipping event..." );
      return StatusCode::SUCCESS;
    }
  }
  m_EventSelection_Cut_Flow->Fill(1);

  // retrieve the vertex container from StoreGate
  const VxContainer *vxContainer;
  if (evtStore()->contains<VxContainer>("VxPrimaryCandidate")) {    
    if( m_storeGate->retrieve(vxContainer,"VxPrimaryCandidate").isFailure() ) {
      ATH_MSG_WARNING( "Can't retrieve vertex container from StoreGate." ); 
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_DEBUG("StoreGate doesn't contain primary vertex container");
    return StatusCode::SUCCESS;
  }
  // Check that you got at least one primary vertex from StoreGate  
  if( vxContainer->size() == 0 ) {
    ATH_MSG_WARNING( "You have lack of primary vertices." );
    return StatusCode::SUCCESS;
  }  
  m_EventSelection_Cut_Flow->Fill(2);

  bool isCollisionCandidate=false;
  for (VxContainer::const_iterator it = vxContainer->begin() ; it != vxContainer->end() ; ++it ){
    const Trk::RecVertex recVertex = (*it)->recVertex();    
    // N tracks at vertex
    std::vector<Trk::VxTrackAtVertex*> *vxTrackAtVertex = (*it)->vxTrackAtVertex();
    if (vxTrackAtVertex->size()>=3) {
      ATH_MSG_DEBUG( "Found a PV with "<<vxTrackAtVertex->size()<<" tracks" ); 
      isCollisionCandidate=true;
      break;
    }
  }
  if (isCollisionCandidate==false) {
    ATH_MSG_INFO( "Didn't find any good vertices, skipping event" ); 	  
    return StatusCode::SUCCESS;
  }
  m_EventSelection_Cut_Flow->Fill(3);

  const Analysis::MuonContainer* importedMuonContainer;
  StatusCode sc = StatusCode::SUCCESS;
  sc=m_storeGate->retrieve(importedMuonContainer, m_MuonContainer );
  if (sc.isFailure() || !importedMuonContainer) {
    ATH_MSG_ERROR( "No MuonCollection found in StoreGate" );
    importedMuonContainer=0;
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Retrieved "<< m_MuonContainer );
  ATH_MSG_DEBUG( "Found "<< importedMuonContainer->size() << " muons in container" );
  if (importedMuonContainer->size()==0) return StatusCode::SUCCESS;
  m_EventSelection_Cut_Flow->Fill(4);

  //============================
  // Selection of tag muons:
  /* We select combined muons as good tag muons, because they have an inner detector track associated with a muon spectrometer track */
  //============================

  std::vector<const Rec::TrackParticle *> myTagMuons; 
  Analysis::MuonContainer::const_iterator muItr;

  // Loop on Staco/Muid collections, keep only combined (high-pt) muons, apply preselection cut on pt and eta, trigger matching requirements
  for ( muItr=importedMuonContainer->begin(); muItr!=importedMuonContainer->end(); muItr++) {
    
    m_Tag_Cut_Flow -> Fill(0);

    if( (*muItr)->isCombinedMuon() || m_tagCombinedMuonOnly==false) {
      m_Tag_Cut_Flow->Fill(1);
  
      if ( (*muItr)->pt()>m_cut_tagMuonPt ) {
	m_Tag_Cut_Flow->Fill(2);

	if ( (*muItr)->eta()<m_cut_muonEta ) {
	  m_Tag_Cut_Flow->Fill(3);
  
	  if ((*muItr)->hasInDetTrackParticle()) {
	    if (isGoodTrack((*muItr)->inDetTrackParticle())) {
	      m_Tag_Cut_Flow->Fill(4);
	      
	      if (m_doTrigger) {
		if (m_tagMatchL1!="" && m_trigEffJpsiTools->isTriggeredMuonL1((*muItr)->inDetTrackParticle(),m_tagMatchL1)==false) {
		  ATH_MSG_DEBUG("Muon not passing L1 matching requirement"); continue;
		}
		m_Tag_Cut_Flow->Fill(5);
		
		if (m_tagMatchL2!="" && m_trigEffJpsiTools->isTriggeredMuonL2((*muItr)->inDetTrackParticle(),m_tagMatchL2)==false) {
		  ATH_MSG_DEBUG("Muon not passing L2 matching requirement"); continue;
		}
		m_Tag_Cut_Flow->Fill(6);
		
		if (m_tagMatchEF!="" && m_trigEffJpsiTools->isTriggeredMuonEF((*muItr)->inDetTrackParticle(),m_tagMatchEF)==false) {
		  ATH_MSG_DEBUG("Muon not passing EF match requirement"); continue;
		}
		m_Tag_Cut_Flow->Fill(7);

		ATH_MSG_DEBUG("Muon passed all trigger matching requirements");
		myTagMuons.push_back((*muItr)->inDetTrackParticle());	      
	      }
	      else {
		m_Tag_Cut_Flow->Fill(5); m_Tag_Cut_Flow->Fill(6); m_Tag_Cut_Flow->Fill(7);
		myTagMuons.push_back((*muItr)->inDetTrackParticle());
	      }
	    }
	  }
	}
      }
    }
  }
  ATH_MSG_DEBUG( "Got "<< myTagMuons.size() << " tag muons after preselection" );
  
  if (myTagMuons.size()==0) {
    ATH_MSG_DEBUG( "Can't find good tag muons. Skipping event..." );
    return StatusCode::SUCCESS;
  }

  // Get Inner Detector tracks from StoreGate  
  const Rec::TrackParticleContainer *importedIDTrackContainer;
  sc = m_storeGate->retrieve(importedIDTrackContainer, m_InnerTrackContainer);  
  if( sc.isFailure() || importedIDTrackContainer == 0) {
    ATH_MSG_ERROR( "No ID Track Collection found in Storegate!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "Retrieved "<< m_InnerTrackContainer );
  ATH_MSG_DEBUG( "Found "<< importedIDTrackContainer->size() << " ID tracks in container" );

  std::vector<const Rec::TrackParticle *> myIDTracks;
  Rec::TrackParticleContainer::const_iterator trkItr;
  for (trkItr=importedIDTrackContainer->begin(); trkItr!=importedIDTrackContainer->end(); trkItr++) {
    if ( (*trkItr)->pt() > m_cut_probeMuonPt ) {
      if ( (*trkItr)->eta() < m_cut_muonEta ) {
	if (isGoodTrack(*trkItr)) {
	  myIDTracks.push_back((*trkItr));
	  m_TagAndProbe_ID_TrackPt->Fill((*trkItr)->pt());
	}
      }
    }
  }
  ATH_MSG_DEBUG( "Got " << myIDTracks.size() << " ID tracks after preselection" );

  /*
  //Truth_____________________
  BPhysToolBox myBPhysToolBox;
  myBPhysToolBox.setTrackParticleContainer(importedIDTrackContainer);
  const TrackParticleTruthCollection* importedTruthColl=0; //links to MC truth for each  particle 
  if (m_doTruth) {
  StatusCode scTruth = StatusCode::SUCCESS;
  scTruth = m_storeGate->retrieve(importedTruthColl,m_TrackParticleTruthCollection);
  if (scTruth.isFailure() || 0 == importedTruthColl) {
  ATH_MSG_ERROR( "No TrackParticleTruthCollection found in StoreGate" );
  importedTruthColl=0;
  return scTruth;
  }
  myBPhysToolBox.setTrackParticleTruthCollection(importedTruthColl);
  }
  // Get McEventCollection from StoreGate
  const McEventCollection* importedMcEventCollection =0; //HEPMC Record for each event
  if (m_doTruth) {
  StatusCode scTruth = StatusCode::SUCCESS;
  scTruth = m_storeGate->retrieve(importedMcEventCollection,m_McEventCollection);
  if (scTruth.isFailure()) {
  ATH_MSG_ERROR( "No McEventCollection available in StoreGate" );
  return scTruth;
  }
  myBPhysToolBox.setMcEventCollection(importedMcEventCollection);
  }
  // Get track truth from StoreGate
  const TruthParticleContainer* importedTruthPartContainer=0; //links to MC truth for each  particle 
  if (m_doTruth) {
  StatusCode scTruth = StatusCode::SUCCESS;
  scTruth = m_storeGate->retrieve(importedTruthPartContainer,m_TruthParticleContainer);
  if (scTruth.isFailure() || 0 == importedTruthPartContainer ) {
  ATH_MSG_ERROR( "No TruthParticleContainer found in StoreGate" );
  importedTruthColl=0;
  return scTruth;
  }
  myBPhysToolBox.setPartTruthCollection(importedTruthPartContainer);
  }
  //Truth particles
  const HepMC::GenParticle* jpsi;
  const HepMC::GenParticle* muPlus;
  const HepMC::GenParticle* muMinus;
  std::vector<std::vector<const Rec::TrackParticle*> > signalTracksHolder; // Holder for tracks orginating from signal decays
  std::vector<const Rec::TrackParticle*> eventTracks;
  std::vector<std::vector<const HepMC::GenParticle*> > trueParticlesHolder;
  std::vector<const Rec::TrackParticle*> muonPlusTracks;
  std::vector<const Rec::TrackParticle*> muonMinusTracks;
  std::vector<const Rec::TrackParticle*> muonsTruth;

  if (m_doTruth) {
  // Decay topology control
  std::vector<int> decayTopology; 
  decayTopology.push_back(443); decayTopology.push_back(-13); decayTopology.push_back(13);
  // Find the HepMC::GenParticles originating from this decay
  trueParticlesHolder = myBPhysToolBox.getSignalParticles(decayTopology);
  ATH_MSG_DEBUG( "Number of Jpsi decays in truth: " << trueParticlesHolder.size() );
  // Find the final state ID tracks originating from this decay   
  signalTracksHolder = myBPhysToolBox.getSignalTracks(decayTopology);
  ATH_MSG_DEBUG( "Number of signal decays: "<< signalTracksHolder.size() );
  if (trueParticlesHolder.size()==1) {
  if ( trueParticlesHolder[0].size()==3) {
  for (std::vector<const HepMC::GenParticle*>::iterator truePtclItr=trueParticlesHolder[0].begin(); truePtclItr!=trueParticlesHolder[0].end(); ++truePtclItr) {
  if ((*truePtclItr)->pdg_id() == 443) jpsi = (*truePtclItr);
  if ((*truePtclItr)->pdg_id() == -13) muMinus = (*truePtclItr);
  if ((*truePtclItr)->pdg_id() == 13) muPlus = (*truePtclItr);
  }
  }
  } else {
  ATH_MSG_WARNING( "Truth record contains " << trueParticlesHolder.size() << " signal processes. Skipping event." );
  return StatusCode::SUCCESS;
  }  
  // Check number of tracks
  if (signalTracksHolder.size()==1) {
  if(signalTracksHolder[0].size()>=2) {
  // Check content of the true tracks
  for(std::vector<const Rec::TrackParticle*>::iterator sigTrkItr=signalTracksHolder[0].begin(); sigTrkItr!=signalTracksHolder[0].end(); ++sigTrkItr) {
  HepMC::GenParticle* tmpP = myBPhysToolBox.getTrkGenParticle(*sigTrkItr);
  if (tmpP!=NULL) {
  if ( tmpP->pdg_id() == 13)
  muonPlusTracks.push_back(*sigTrkItr);
  if ( tmpP->pdg_id() == -13)
  muonMinusTracks.push_back(*sigTrkItr);
  }
  }
  }
  }
  if ( (muonPlusTracks.size() == 1) && (muonMinusTracks.size() == 1) ) {
  ATH_MSG_DEBUG( "Got a signal J/psi" );
  ++nJpsiTruth;
  muonsTruth.push_back( muonPlusTracks[0] );
  muonsTruth.push_back( muonMinusTracks[0] );
  }
  }
  //__________________________
  */

  //Make pairs of each tag muon with oppositely charged inner detector tracks
  double charge1=0.;
  double charge2=0.;
  std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > myPairs;
  std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > myPairsSS;
  for (unsigned int i=0; i<myTagMuons.size(); i++) {
    charge1 = myTagMuons[i]->charge();
    ATH_MSG_DEBUG( "Got muon with q=" <<charge1);
    for (unsigned int j=0; j<myIDTracks.size(); j++) {
      if (myIDTracks[j]==myTagMuons[i]) continue; 
      charge2=myIDTracks[j]->charge();
      ATH_MSG_DEBUG( "Got track with q=" <<charge2);
      m_TagAndProbe_Charge->Fill(charge1*charge2);
      
      if (charge1*charge2<0) {
	ATH_MSG_DEBUG( "Got pair of oppositely charged tracks" );
	//sort pair, ***tag goes first***
	myPairs.push_back(std::make_pair(myTagMuons[i],myIDTracks[j]));
      }
      else {
	ATH_MSG_DEBUG( "Got pair of same sign tracks" );
	//sort pair, ***tag goes first***
	myPairsSS.push_back(std::make_pair(myTagMuons[i],myIDTracks[j]));
      }
    }
  }
  ATH_MSG_DEBUG( "Found " << myPairs.size() << " pairs" );
  ATH_MSG_DEBUG( "Found " << myPairsSS.size() << "same sign pairs" );
  if (myPairs.size()==0 && myPairsSS.size()==0 ) {
    ATH_MSG_DEBUG( "No pairs, skipping event..." );
    return StatusCode::SUCCESS;
  }
  
  //select Jpsi candidates
  int nJpsi=0;
  //double pt1=0.;
  //double pt2=0.;
  
  std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > myJpsiMuons;
  //  if (myPairs.size()>0) m_TagAndProbe_Cut_Flow -> Fill(0);
  
  for (unsigned int i=0; i<myPairs.size(); i++) {
    m_TagAndProbe_Cut_Flow->Fill(1);
    ATH_MSG_DEBUG( "Getting pair #"<< (i+1) );
    
    const Rec::TrackParticle *track1 = myPairs[i].first;
    const Rec::TrackParticle *track2 = myPairs[i].second;
    
   // pt1 = track1->pt();
   // pt2 = track2->pt();

    double invMass = CorrectInvMassTwoMuons(track1,track2);
    ATH_MSG_DEBUG( "Mass: " << invMass );

    //Keep quantities before cuts
    m_TagAndProbe_ID_diMuonMass->Fill(invMass);
    
    //quantities after vertexing
    double refMass=-999;
    double tau=-999.;
    double vtxChi2=-999.;
    
    //apply cuts
    ATH_MSG_DEBUG( "Applying cuts..." );
    if (doVertexing(track1, track2,  (*(vxContainer->begin()))->recVertex(), vtxChi2, refMass, tau) ) {
      m_TagAndProbe_Cut_Flow->Fill(2);
      ATH_MSG_DEBUG( "Successful vertexing..." );
      ATH_MSG_DEBUG( "Chi2: " << vtxChi2 << ", Mass:" << refMass << ", tau:" << tau );
      m_TagAndProbe_ID_diMuonMass_AfterVtx->Fill(invMass);

      double deltaR = m_analysisTools->deltaR(track1,track2);

      m_TagAndProbe_Propertime->Fill(tau);
      m_TagAndProbe_VertexChi2->Fill(vtxChi2);
      m_TagAndProbe_DeltaR->Fill(deltaR);
        
      if (vtxChi2 < m_cut_JpsiVertexChi2 || m_cut_JpsiVertexChi2==0) { 
	m_TagAndProbe_Cut_Flow->Fill(3);
	  
	if ( (deltaR<m_cut_deltaRMax && deltaR>m_cut_deltaRMin) || (m_cut_deltaRMin==0 && m_cut_deltaRMax==0) ) { 
	  m_TagAndProbe_Cut_Flow->Fill(4);
	  //m_TagAndProbe_ID_diMuonMass_Refitted->Fill(refMass);

	  if ( fabs(tau) < m_cut_properTime || m_cut_properTime==0) {
	    m_TagAndProbe_Cut_Flow->Fill(5);

	    if (!isElectron(track1) && !isElectron(track2) ) {
	      m_TagAndProbe_Cut_Flow->Fill(6);
	      
	      ATH_MSG_DEBUG( "Got a tag & probe pair" );
	      nJpsi++;
	      m_TagAndProbe_InvariantMass->Fill(invMass);
	      //m_TagAndProbe_ID_Selected_RefJpsiMass->Fill(refMass);
	      myJpsiMuons.push_back(std::make_pair(track1,track2));
	      
	    }
	  }
	}
      }
    }
    else ATH_MSG_DEBUG( "Vertexing failed" );

  }//close for
  
  m_TagAndProbe_ID_NCandidates->Fill(nJpsi);
  /*
    if (myJpsiMuons.size()==0) {
    ATH_MSG_DEBUG( "No J/psi candidates" );
    return StatusCode::SUCCESS;
    } 
  */ 
  ATH_MSG_INFO( "Found " << nJpsi << " J/psi candidates" );
  nJpsiReco +=nJpsi;

  /*
  //==== matching of Jpsi muons to truth ===========
  std::vector<bool> isTrueJpsi;
  if (m_doTruth) {
  for (unsigned int i=0; i<myJpsiMuons.size(); i++) {
  std::vector<const Rec::TrackParticle *> muons;
  muons.push_back(myJpsiMuons[i].first);
  muons.push_back(myJpsiMuons[i].second);
  if ( myBPhysToolBox.isIdenticalCollection(muons,muonsTruth) ) {
  ATH_MSG_DEBUG( "J/psi matched with truth" );
  nJpsiMatch++;
  isTrueJpsi.push_back(true);
  }
  else isTrueJpsi.push_back(false);
  }
  }
  //================================================
  */
  //Probing track2 in Muon Spectrometer and L1/L2 trigger
  for (unsigned int i=0; i<myJpsiMuons.size(); i++) {
    const Rec::TrackParticle *tag = myJpsiMuons[i].first;
    const Rec::TrackParticle *probe = myJpsiMuons[i].second;
    double invMass = CorrectInvMassTwoMuons(tag,probe);
    ATH_MSG_DEBUG( "Muon-track mass: " << invMass );

    //all probe muons
    m_TagAndProbe_Mass_BeforeProbeAssociation->Fill(invMass);

    if (!m_doBkgExtraction) {
      m_TagAndProbe_Ntotal_vs_Eta->Fill(probe->eta());
      m_TagAndProbe_Ntotal_vs_Phi->Fill(probe->phi());
      m_TagAndProbe_Ntotal_vs_Pt ->Fill(probe->pt());
    }

    //find region
    if ( fabs(m_jpsiMass-invMass)<3*m_jpsiSigma ) {
      ATH_MSG_DEBUG( "invariant mass value in signal region" );
      m_TagAndProbe_Cut_Flow -> Fill(7);
      m_TagAndProbe_Mass_BeforeProbeAssociation_SR->Fill(invMass);
      m_TagAndProbe_Pt_BeforeProbeAssociation_SR->Fill(probe->pt());
      m_TagAndProbe_Eta_BeforeProbeAssociation_SR->Fill(probe->eta());
      m_TagAndProbe_Phi_BeforeProbeAssociation_SR->Fill(probe->phi());
    }
    else if (m_doBkgExtraction) {
      if ( invMass>(m_jpsiMass+5*m_jpsiSigma) && invMass<(m_jpsiMass+11*m_jpsiSigma) ) {
	ATH_MSG_DEBUG( "invariant mass value in right sideband" );
	m_TagAndProbe_Mass_BeforeProbeAssociation_R->Fill(invMass);
	m_TagAndProbe_Pt_BeforeProbeAssociation_R->Fill(probe->pt());
	m_TagAndProbe_Eta_BeforeProbeAssociation_R->Fill(probe->eta());
	m_TagAndProbe_Phi_BeforeProbeAssociation_R->Fill(probe->phi());
      }
      else if ( invMass>(m_jpsiMass-11*m_jpsiSigma) && invMass<(m_jpsiMass-5*m_jpsiSigma) ) {
	ATH_MSG_DEBUG( "invariant mass value in left sideband" );
	m_TagAndProbe_Mass_BeforeProbeAssociation_L->Fill(invMass);
	m_TagAndProbe_Pt_BeforeProbeAssociation_L->Fill(probe->pt());
	m_TagAndProbe_Eta_BeforeProbeAssociation_L->Fill(probe->eta());
	m_TagAndProbe_Phi_BeforeProbeAssociation_L->Fill(probe->phi());
      }
    }

    for (unsigned int j=0; j<m_muonCollectionName.size(); j++) {
      
      if (isAssociatedWithMSTrack(probe,m_muonCollectionName[j])) {
	//probe muons reconstructed in MS
	ATH_MSG_DEBUG( "Probe ID muon associated to a MS track");
	m_TagAndProbe_Cut_Flow -> Fill(8);

	if (!m_doBkgExtractionAfterProbeAssociation) {
	  m_TagAndProbe_NpassedReco_vs_Eta[j]->Fill(probe->eta());
	  m_TagAndProbe_NpassedReco_vs_Pt[j]->Fill(probe->pt());
	  m_TagAndProbe_NpassedReco_vs_Phi[j]->Fill(probe->phi());   
	}

	if (m_muonCollectionName[j]=="StacoMuonCollection")
	  m_TagAndProbe_InvariantMass_AfterAssociationSTACO->Fill(invMass);
	else m_TagAndProbe_InvariantMass_AfterAssociationMUID->Fill(invMass);

	m_TagAndProbe_Mass_AfterProbeAssociation[j]->Fill(invMass);
	//find region
	if ( fabs(m_jpsiMass-invMass)<3*m_jpsiSigma ) {
	  ATH_MSG_DEBUG( "invariant mass value in signal region" );	  
	  m_TagAndProbe_Mass_AfterProbeAssociation_SR[j]->Fill(invMass);
	  m_TagAndProbe_Pt_AfterProbeAssociation_SR[j]->Fill(probe->pt());
	  m_TagAndProbe_Eta_AfterProbeAssociation_SR[j]->Fill(probe->eta());
	  m_TagAndProbe_Phi_AfterProbeAssociation_SR[j]->Fill(probe->phi());
	}
	else if (m_doBkgExtractionAfterProbeAssociation) {
	  if ( invMass>(m_jpsiMass+5*m_jpsiSigma) && invMass<(m_jpsiMass+11*m_jpsiSigma) ) {
	    ATH_MSG_DEBUG( "invariant mass value in right sideband" );
	    m_TagAndProbe_Mass_AfterProbeAssociation_R[j]->Fill(invMass);
	    m_TagAndProbe_Pt_AfterProbeAssociation_R[j]->Fill(probe->pt());
	    m_TagAndProbe_Eta_AfterProbeAssociation_R[j]->Fill(probe->eta());
	    m_TagAndProbe_Phi_AfterProbeAssociation_R[j]->Fill(probe->phi());
	  }
	  else if ( invMass>(m_jpsiMass-11*m_jpsiSigma) && invMass<(m_jpsiMass-5*m_jpsiSigma) ) {
	    ATH_MSG_DEBUG( "invariant mass value in left sideband" );
	    m_TagAndProbe_Mass_AfterProbeAssociation_L[j]->Fill(invMass);
	    m_TagAndProbe_Pt_AfterProbeAssociation_L[j]->Fill(probe->pt());
	    m_TagAndProbe_Eta_AfterProbeAssociation_L[j]->Fill(probe->eta());
	    m_TagAndProbe_Phi_AfterProbeAssociation_L[j]->Fill(probe->phi());
	  }
	}
      }
    }
    /*
      if (m_doTruth && isTrueJpsi[i]) {
      m_h_etatruth->Fill(probe->eta(), 1.);
      m_h_pttruth->Fill(probe->pt(), 1.);
      m_h_phitruth->Fill(probe->phi(), 1.);
      }
    */
    if (m_doTrigger) {
      bool isTriggeredL1=false;
      bool isTriggeredL2=false;

      if (m_doTriggerEffL1) {
	for (int itrig=0; itrig<2; itrig++) {
	  //probe muons fired the L1 trigger?
	  if (m_trigEffJpsiTools->isTriggeredMuon(probe,itrig+1) ) { //1-> MU4, 2-> MU6
	    isTriggeredL1=true;
	    m_TagAndProbe_NpassedL1_vs_Eta[itrig]->Fill(probe->eta() , 1.);
	    m_TagAndProbe_NpassedL1_vs_Pt[itrig]->Fill(probe->pt() , 1.);
	    m_TagAndProbe_NpassedL1_vs_Phi[itrig]->Fill(probe->phi(), 1.);
	  }
	}
      }
      if (isTriggeredL1) m_TagAndProbe_Cut_Flow -> Fill(9);

      if (m_doTriggerEffL2) {
	for (unsigned int itrig=0; itrig<m_trigMenus.size(); itrig++) {	  
	  //probe muons fired the L2 trigger?
	  if (m_trigDecTool->isPassed(m_trigMenus[itrig])) {	    
	    //check if muon is matched at L1 as well - NOT GOOD
	    /* if(  (m_trigMenus[itrig].find("mu4")<m_trigMenus[itrig].length() && m_trigEffJpsiTools->isTriggeredMuon(probe,1) ) || 
	       (m_trigMenus[itrig].find("mu6")<m_trigMenus[itrig].length() && m_trigEffJpsiTools->isTriggeredMuon(probe,2) ) ) */
	    isTriggeredL2=true;
	    m_TagAndProbe_NpassedL2_vs_Eta[itrig]->Fill(probe->eta() , 1.);
	    m_TagAndProbe_NpassedL2_vs_Pt[itrig]->Fill(probe->pt() , 1.);
	    m_TagAndProbe_NpassedL2_vs_Phi[itrig]->Fill(probe->phi(), 1.);	    
	  }
	}
      }
      if (isTriggeredL2) m_TagAndProbe_Cut_Flow -> Fill(9);	
    }
  }
  
  //-----------------------------------------------
  //____________________________ same sign analysis
  //-----------------------------------------------
  ATH_MSG_DEBUG( "SS analysis" );
  std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > myJpsiMuonsSS;
  
  for (unsigned int i=0; i<myPairsSS.size(); i++) {
    ATH_MSG_DEBUG( "Getting pair #"<< (i+1) );
    
    const Rec::TrackParticle *track1 = myPairsSS[i].first;
    const Rec::TrackParticle *track2 = myPairsSS[i].second;
    
   // pt1 = track1->pt();
   // pt2 = track2->pt();

    double invMass = CorrectInvMassTwoMuons(track1,track2);
    ATH_MSG_DEBUG( "Mass: " << invMass );

    //quantities after vertexing
    double refMass=-999;
    double tau=-999.;
    double vtxChi2=-999.;
    
    //apply cuts
    ATH_MSG_DEBUG( "Applying cuts..." );
    if (doVertexing(track1, track2,  (*(vxContainer->begin()))->recVertex(), vtxChi2, refMass, tau) ) {
      ATH_MSG_DEBUG( "Successful vertexing..." );
      ATH_MSG_DEBUG( "Chi2: " << vtxChi2 << ", Mass:" << refMass << ", tau:" << tau );
      double deltaR = m_analysisTools->deltaR(track1,track2);
      if (vtxChi2 < m_cut_JpsiVertexChi2 || m_cut_JpsiVertexChi2==0) { 
	if ( (deltaR<m_cut_deltaRMax && deltaR>m_cut_deltaRMin) || (m_cut_deltaRMin==0 && m_cut_deltaRMax==0) ) { 
	  if ( fabs(tau) < m_cut_properTime || m_cut_properTime==0) {
	    if (!isElectron(track1) && !isElectron(track2) ) {
	      ATH_MSG_DEBUG( "Got a tag & probe pair" );
	      myJpsiMuonsSS.push_back(std::make_pair(track1,track2));	      
	    }
	  }
	}
      }
    }
    else ATH_MSG_DEBUG( "Vertexing failed" );
  }//close for
  
  ATH_MSG_INFO( "Found " << nJpsi << " J/psi SS candidates" );

  //Probing track2 in Muon Spectrometer and L1/L2 trigger
  for (unsigned int i=0; i<myJpsiMuonsSS.size(); i++) {
    const Rec::TrackParticle *tag = myJpsiMuonsSS[i].first;
    const Rec::TrackParticle *probe = myJpsiMuonsSS[i].second;
    double invMass = CorrectInvMassTwoMuons(tag,probe);
    ATH_MSG_DEBUG( "Muon-track mass: " << invMass );

    //all probe muons
    m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation->Fill(invMass);
    
    //find region
    if ( fabs(m_jpsiMass-invMass)<3*m_jpsiSigma ) {
      ATH_MSG_DEBUG( "invariant mass value in signal region" );
      m_TagAndProbe_Cut_Flow -> Fill(7);
      m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_SR->Fill(invMass);
      m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_SR->Fill(probe->pt());
      m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_SR->Fill(probe->eta());
      m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_SR->Fill(probe->phi());
    }
    else if (m_doBkgExtraction) {
      if ( invMass>(m_jpsiMass+5*m_jpsiSigma) && invMass<(m_jpsiMass+11*m_jpsiSigma) ) {
	ATH_MSG_DEBUG( "invariant mass value in right sideband" );
	m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_R->Fill(invMass);
	m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_R->Fill(probe->pt());
	m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_R->Fill(probe->eta());
	m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_R->Fill(probe->phi());
      }
      else if ( invMass>(m_jpsiMass-11*m_jpsiSigma) && invMass<(m_jpsiMass-5*m_jpsiSigma) ) {
	ATH_MSG_DEBUG( "invariant mass value in left sideband" );
	m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation_L->Fill(invMass);
	m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_L->Fill(probe->pt());
	m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_L->Fill(probe->eta());
	m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_L->Fill(probe->phi());
      }
    }

    for (unsigned int j=0; j<m_muonCollectionName.size(); j++) {

      if (isAssociatedWithMSTrack(probe,m_muonCollectionName[j])) {
	//probe muons reconstructed in MS
	ATH_MSG_DEBUG( "Probe ID muon associated to a MS track");
	if (m_muonCollectionName[j]=="StacoMuonCollection")
	  m_TagAndProbe_InvariantMass_SameSign_AfterAssociationSTACO->Fill(invMass);
	else m_TagAndProbe_InvariantMass_SameSign_AfterAssociationMUID->Fill(invMass);

	m_TagAndProbe_Mass_SameSign_AfterProbeAssociation[j]->Fill(invMass);
	
	//find region
	if ( fabs(m_jpsiMass-invMass)<3*m_jpsiSigma ) {
	  ATH_MSG_DEBUG( "invariant mass value in signal region" );	  
	  m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_SR[j]->Fill(invMass);
	  m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_SR[j]->Fill(probe->pt());
	  m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_SR[j]->Fill(probe->eta());
	  m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_SR[j]->Fill(probe->phi());
	}
	else if (m_doBkgExtractionAfterProbeAssociation) {
	  if ( invMass>(m_jpsiMass+5*m_jpsiSigma) && invMass<(m_jpsiMass+11*m_jpsiSigma) ) {
	    ATH_MSG_DEBUG( "invariant mass value in right sideband" );
	    m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_R[j]->Fill(invMass);
	    m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_R[j]->Fill(probe->pt());
	    m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_R[j]->Fill(probe->eta());
	    m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_R[j]->Fill(probe->phi());
	  }
	  else if ( invMass>(m_jpsiMass-11*m_jpsiSigma) && invMass<(m_jpsiMass-5*m_jpsiSigma) ) {
	    ATH_MSG_DEBUG( "invariant mass value in left sideband" );
	    m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_L[j]->Fill(invMass);
	    m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_L[j]->Fill(probe->pt());
	    m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_L[j]->Fill(probe->eta());
	    m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_L[j]->Fill(probe->phi());
	  }
	}
      }
    }
  }
  
  return sc;
} //==================================end of fillHistograms()==============================================================
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode JpsiMuonInsituPerformance::procHistograms()
{
  ATH_MSG_DEBUG( "in procHistograms()" );

  if(endOfEventsBlock){}
  if(endOfLumiBlock){}

  if(endOfRun){
    ATH_MSG_DEBUG( "isEndOfRun" );

    int N_SR = m_TagAndProbe_Mass_BeforeProbeAssociation_SR->GetEntries(); //number of events in signal region
    ATH_MSG_INFO( "Number of events in signal region (J/psi candidates): " << N_SR );

    if (m_doBkgExtraction && N_SR>0) {
      /////////////////////////////////////////////////////////////////       
      ATH_MSG_DEBUG( "background extraction with using sidebands" );//
      ///////////////////////////////////////////////////////////////
      int nBins_SR = 2.*m_jpsiSignalRegion*m_jpsiSigma/m_jpsiMassBinning; //number of bins in signal region
      int nBins_SB = m_jpsiSideband*m_jpsiSigma/m_jpsiMassBinning;       //number of bins in each sideband
      int Nbkg_L = m_TagAndProbe_Mass_BeforeProbeAssociation_L->GetEntries(); //number of bkg events from left sideband
      int Nbkg_R = m_TagAndProbe_Mass_BeforeProbeAssociation_R->GetEntries(); //number of bkg events from right sideband
      double Nbkg_SR = 0.5*(Nbkg_L+Nbkg_R)*nBins_SR/nBins_SB; //number of BKG events in signal region
      double Nsig_SR = N_SR-Nbkg_SR;    
      double MF=Nsig_SR/N_SR;
      ATH_MSG_DEBUG( "nBins_SR: " << nBins_SR << " ,  nBins_SB: " << nBins_SB << " ,  Nbkg_L: " << Nbkg_L << " ,  Nbkg_R: " << Nbkg_R << " ,  Nbkg_SR: " << Nbkg_SR << " ,  MF: " << MF );
      ATH_MSG_INFO( "After background extraction (J/psi signal): " << Nsig_SR );

      m_TagAndProbe_PtBkg_BeforeProbeAssociation_SR->Add(m_TagAndProbe_Pt_BeforeProbeAssociation_L,m_TagAndProbe_Pt_BeforeProbeAssociation_R,0.5*MF,0.5*MF);
      m_TagAndProbe_PtBkgExtracted_BeforeProbeAssociation_SR->Add(m_TagAndProbe_Pt_BeforeProbeAssociation_SR, m_TagAndProbe_PtBkg_BeforeProbeAssociation_SR, 1,-1);

      m_TagAndProbe_EtaBkg_BeforeProbeAssociation_SR->Add(m_TagAndProbe_Eta_BeforeProbeAssociation_L,m_TagAndProbe_Eta_BeforeProbeAssociation_R,0.5*MF,0.5*MF);
      m_TagAndProbe_EtaBkgExtracted_BeforeProbeAssociation_SR->Add(m_TagAndProbe_Eta_BeforeProbeAssociation_SR, m_TagAndProbe_EtaBkg_BeforeProbeAssociation_SR, 1,-1);

      m_TagAndProbe_PhiBkg_BeforeProbeAssociation_SR->Add(m_TagAndProbe_Phi_BeforeProbeAssociation_L,m_TagAndProbe_Phi_BeforeProbeAssociation_R,0.5*MF,0.5*MF);
      m_TagAndProbe_PhiBkgExtracted_BeforeProbeAssociation_SR->Add(m_TagAndProbe_Phi_BeforeProbeAssociation_SR, m_TagAndProbe_PhiBkg_BeforeProbeAssociation_SR, 1,-1);
    
      /*
	m_TagAndProbe_MassBkg_BeforeProbeAssociation_SR->Add(m_TagAndProbe_Mass_BeforeProbeAssociation_L,m_TagAndProbe_Mass_BeforeProbeAssociation_R,0.5,0.5);
	m_TagAndProbe_MassBkgExtracted_BeforeProbeAssociation_SR->Add(m_TagAndProbe_Mass_BeforeProbeAssociation_SR,m_TagAndProbe_MassBkg_BeforeProbeAssociation_SR,1,-1);
      */

      m_TagAndProbe_Ntotal_vs_Eta = m_TagAndProbe_EtaBkgExtracted_BeforeProbeAssociation_SR; 
      m_TagAndProbe_Ntotal_vs_Phi = m_TagAndProbe_PhiBkgExtracted_BeforeProbeAssociation_SR; 
      m_TagAndProbe_Ntotal_vs_Pt = m_TagAndProbe_PtBkgExtracted_BeforeProbeAssociation_SR; 
    }


    if (m_doEfficiencies) {
      for (unsigned int j=0; j<m_muonCollectionName.size(); j++) {
	m_TagAndProbe_Efficiency_Eta[j]->Divide(m_TagAndProbe_NpassedReco_vs_Eta[j],m_TagAndProbe_Ntotal_vs_Eta);
	//m_TagAndProbe_Efficiency_Eta[j]->SetMarkerStyle(22);
	//m_TagAndProbe_Efficiency_Eta[j]->SetMarkerSizee(1.2);
	//m_TagAndProbe_Efficiency_Eta[j]->SetOption("P");
	m_TagAndProbe_Efficiency_Eta[j]->GetYaxis()->SetRangeUser(0.,1.1);
      
	m_TagAndProbe_Efficiency_Phi[j]->Divide(m_TagAndProbe_NpassedReco_vs_Phi[j],m_TagAndProbe_Ntotal_vs_Phi);
	//	m_TagAndProbe_Efficiency_Phi[j]->SetMarkerStyle(22);
	//m_TagAndProbe_Efficiency_Phi[j]->SetOption("P");
	m_TagAndProbe_Efficiency_Phi[j]->GetYaxis()->SetRangeUser(0.,1.1);
	
	m_TagAndProbe_Efficiency_Pt[j]->Divide(m_TagAndProbe_NpassedReco_vs_Pt[j],m_TagAndProbe_Ntotal_vs_Pt);
	//m_TagAndProbe_Efficiency_Pt[j]->SetMarkerStyle(22);
	//m_TagAndProbe_Efficiency_Pt[j]->SetOption("P");
	m_TagAndProbe_Efficiency_Pt[j]->GetYaxis()->SetRangeUser(0.,1.1);	
      }
      /*
	if (m_doTrigger) {
	for (int itrig=0; itrig<2; itrig++) { 
	if (m_doTriggerEffL1) {
	setEfficiencyHist(m_TagAndProbe_L1TriggerEfficiency_Eta[itrig],m_TagAndProbe_NpassedL1_vs_Eta[itrig],m_TagAndProbe_Ntotal_vs_Eta);
	setEfficiencyHist(m_TagAndProbe_L1TriggerEfficiency_Phi[itrig],m_TagAndProbe_NpassedL1_vs_Phi[itrig],m_TagAndProbe_Ntotal_vs_Phi);
	setEfficiencyHist(m_TagAndProbe_L1TriggerEfficiency_Pt[itrig],m_TagAndProbe_NpassedL1_vs_Pt[itrig],m_TagAndProbe_Ntotal_vs_Pt);
	if (m_doTriggerEffL1) {
	m_TagAndProbe_L1TriggerEfficiency_Eta[itrig]->Divide(m_TagAndProbe_NpassedL1_vs_Eta[itrig],m_TagAndProbe_Ntotal_vs_Eta);
	m_TagAndProbe_L1TriggerEfficiency_Eta[itrig]->SetMarkerStyle(2);
	m_TagAndProbe_L1TriggerEfficiency_Eta[itrig]->SetOption("P");
	m_TagAndProbe_L1TriggerEfficiency_Eta[itrig]->GetYaxis()->SetRangeUser(0.,1.1);
	    
	m_TagAndProbe_L1TriggerEfficiency_Phi[itrig]->Divide(m_TagAndProbe_NpassedL1_vs_Phi[itrig],m_TagAndProbe_Ntotal_vs_Phi);
	m_TagAndProbe_L1TriggerEfficiency_Phi[itrig]->SetMarkerStyle(2);
	m_TagAndProbe_L1TriggerEfficiency_Phi[itrig]->SetOption("P");
	m_TagAndProbe_L1TriggerEfficiency_Phi[itrig]->GetYaxis()->SetRangeUser(0.,1.1);
	    
	m_TagAndProbe_L1TriggerEfficiency_Pt[itrig]->Divide(m_TagAndProbe_NpassedL1_vs_Pt[itrig],m_TagAndProbe_Ntotal_vs_Pt);
	m_TagAndProbe_L1TriggerEfficiency_Pt[itrig]->SetMarkerStyle(2);
	m_TagAndProbe_L1TriggerEfficiency_Pt[itrig]->SetOption("P");
	m_TagAndProbe_L1TriggerEfficiency_Pt[itrig]->GetYaxis()->SetRangeUser(0.,1.1);

	}
	if (m_doTriggerEffL2) {
	setEfficiencyHist(m_TagAndProbe_L2TriggerEfficiency_Eta[itrig],m_TagAndProbe_NpassedL2_vs_Eta[itrig],m_TagAndProbe_Ntotal_vs_Eta);
	setEfficiencyHist(m_TagAndProbe_L2TriggerEfficiency_Phi[itrig],m_TagAndProbe_NpassedL2_vs_Phi[itrig],m_TagAndProbe_Ntotal_vs_Phi);
	setEfficiencyHist(m_TagAndProbe_L2TriggerEfficiency_Pt[itrig],m_TagAndProbe_NpassedL2_vs_Pt[itrig],m_TagAndProbe_Ntotal_vs_Pt);
	m_TagAndProbe_L2TriggerEfficiency_Eta[itrig]->Divide(m_TagAndProbe_NpassedL2_vs_Eta[itrig],m_TagAndProbe_Ntotal_vs_Eta);
	m_TagAndProbe_L2TriggerEfficiency_Eta[itrig]->SetMarkerStyle(2);
	m_TagAndProbe_L2TriggerEfficiency_Eta[itrig]->SetOption("P");
	m_TagAndProbe_L2TriggerEfficiency_Eta[itrig]->GetYaxis()->SetRangeUser(0.,1.1);
	    
	m_TagAndProbe_L2TriggerEfficiency_Phi[itrig]->Divide(m_TagAndProbe_NpassedL2_vs_Phi[itrig],m_TagAndProbe_Ntotal_vs_Phi);
	m_TagAndProbe_L2TriggerEfficiency_Phi[itrig]->SetMarkerStyle(2);
	m_TagAndProbe_L2TriggerEfficiency_Phi[itrig]->SetOption("P");
	m_TagAndProbe_L2TriggerEfficiency_Phi[itrig]->GetYaxis()->SetRangeUser(0.,1.1);
	    
	m_TagAndProbe_L2TriggerEfficiency_Pt[itrig]->Divide(m_TagAndProbe_NpassedL2_vs_Pt[itrig],m_TagAndProbe_Ntotal_vs_Pt);
	m_TagAndProbe_L2TriggerEfficiency_Pt[itrig]->SetMarkerStyle(2);
	m_TagAndProbe_L2TriggerEfficiency_Pt[itrig]->SetOption("P");
	m_TagAndProbe_L2TriggerEfficiency_Pt[itrig]->GetYaxis()->SetRangeUser(0.,1.1);

	}
	}
	}
      */
      /*
	if (m_doTruth) {
	setEfficiencyHist(m_TagAndProbe_Efficiency_Eta_Truth,m_TagAndProbe_NpassedTruth_vs_Eta,m_TagAndProbe_Ntotal_vs_Eta);
	m_TagAndProbe_Efficiency_Eta_Truth->SetMarkerColor(kRed);
      	
	setEfficiencyHist(m_TagAndProbe_Efficiency_Phi_Truth,m_TagAndProbe_NpassedTruth_vs_Phi,m_TagAndProbe_Ntotal_vs_Phi);
	m_TagAndProbe_Efficiency_Phi_Truth->SetMarkerColor(kRed);
	setEfficiencyHist(m_TagAndProbe_Efficiency_Pt_Truth,m_TagAndProbe_NpassedTruth_vs_Pt,m_TagAndProbe_Ntotal_vs_Pt);
	m_TagAndProbe_Efficiency_Pt_Truth->SetMarkerColor(kRed);	
	m_TagAndProbe_Efficiency_Eta_Truth->Divide(m_TagAndProbe_NpassedTruth_vs_Eta,m_TagAndProbe_Ntotal_vs_Eta);
	m_TagAndProbe_Efficiency_Eta_Truth->SetMarkerStyle(3);
	m_TagAndProbe_Efficiency_Eta_Truth->SetMarkerColor(kRed);
	m_TagAndProbe_Efficiency_Eta_Truth->SetOption("P");
	m_TagAndProbe_Efficiency_Eta_Truth->GetYaxis()->SetRangeUser(0.,1.1);
	
	m_TagAndProbe_Efficiency_Phi_Truth->Divide(m_TagAndProbe_NpassedTruth_vs_Phi,m_TagAndProbe_Ntotal_vs_Phi);
	m_TagAndProbe_Efficiency_Phi_Truth->SetMarkerStyle(3);
	m_TagAndProbe_Efficiency_Phi_Truth->SetMarkerColor(kRed);
	m_TagAndProbe_Efficiency_Phi_Truth->SetOption("P");
	m_TagAndProbe_Efficiency_Phi_Truth->GetYaxis()->SetRangeUser(0.,1.1);
	
	m_TagAndProbe_Efficiency_Pt_Truth->Divide(m_TagAndProbe_NpassedTruth_vs_Pt,m_TagAndProbe_Ntotal_vs_Pt);
	m_TagAndProbe_Efficiency_Pt_Truth->SetMarkerStyle(3);
	m_TagAndProbe_Efficiency_Pt_Truth->SetMarkerColor(kRed);
	m_TagAndProbe_Efficiency_Pt_Truth->SetOption("P");
	m_TagAndProbe_Efficiency_Pt_Truth->GetYaxis()->SetRangeUser(0.,1.1);

	}
      */
    }//do Efficiencies

    m_TagAndProbe_Efficiency_Eta.clear();
    m_TagAndProbe_Efficiency_Phi.clear();
    m_TagAndProbe_Efficiency_Pt.clear();

    m_TagAndProbe_NpassedReco_vs_Eta.clear();
    m_TagAndProbe_NpassedReco_vs_Pt.clear();
    m_TagAndProbe_NpassedReco_vs_Phi.clear();

    m_TagAndProbe_NpassedL1_vs_Eta.clear();
    m_TagAndProbe_NpassedL1_vs_Pt.clear();
    m_TagAndProbe_NpassedL1_vs_Phi.clear();

    m_TagAndProbe_NpassedL2_vs_Eta.clear();
    m_TagAndProbe_NpassedL2_vs_Pt.clear();
    m_TagAndProbe_NpassedL2_vs_Phi.clear();

    m_TagAndProbe_NpassedEF_vs_Eta.clear();
    m_TagAndProbe_NpassedEF_vs_Pt.clear();
    m_TagAndProbe_NpassedEF_vs_Phi.clear();

    m_TagAndProbe_Mass_AfterProbeAssociation.clear();
    m_TagAndProbe_Mass_AfterProbeAssociation_L.clear();
    m_TagAndProbe_Mass_AfterProbeAssociation_SR.clear();
    m_TagAndProbe_Mass_AfterProbeAssociation_R.clear();
    m_TagAndProbe_MassBkgExtracted_AfterProbeAssociation.clear();
    m_TagAndProbe_MassBkgExtracted_AfterProbeAssociation_SR.clear();

    m_TagAndProbe_Pt_AfterProbeAssociation_L.clear();
    m_TagAndProbe_Eta_AfterProbeAssociation_L.clear();
    m_TagAndProbe_Phi_AfterProbeAssociation_L.clear();
    
    m_TagAndProbe_Pt_AfterProbeAssociation_SR.clear();
    m_TagAndProbe_Eta_AfterProbeAssociation_SR.clear();
    m_TagAndProbe_Phi_AfterProbeAssociation_SR.clear();
    
    m_TagAndProbe_Pt_AfterProbeAssociation_R.clear();
    m_TagAndProbe_Eta_AfterProbeAssociation_R.clear();
    m_TagAndProbe_Phi_AfterProbeAssociation_R.clear();

    m_TagAndProbe_Mass_SameSign_AfterProbeAssociation.clear();
    m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_L.clear();
    m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_SR.clear();
    m_TagAndProbe_Mass_SameSign_AfterProbeAssociation_R.clear();
    m_TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation.clear();
    m_TagAndProbe_MassBkgExtracted_SameSign_AfterProbeAssociation_SR.clear();

    m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_L.clear();
    m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_L.clear();
    m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_L.clear();
    
    m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_SR.clear();
    m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_SR.clear();
    m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_SR.clear();
    
    m_TagAndProbe_Pt_SameSign_AfterProbeAssociation_R.clear();
    m_TagAndProbe_Eta_SameSign_AfterProbeAssociation_R.clear();
    m_TagAndProbe_Phi_SameSign_AfterProbeAssociation_R.clear();

    
    if (m_doTrigger) {
      if (m_doTriggerEffL1) {
	m_TagAndProbe_L1TriggerEfficiency_Eta.clear();
	m_TagAndProbe_L1TriggerEfficiency_Phi.clear();
	m_TagAndProbe_L1TriggerEfficiency_Pt.clear();
      }
      if (m_doTriggerEffL2) {
	m_TagAndProbe_L2TriggerEfficiency_Eta.clear();
	m_TagAndProbe_L2TriggerEfficiency_Phi.clear();
	m_TagAndProbe_L2TriggerEfficiency_Pt.clear();
      }
      if (m_doTriggerEffEF) {
	//m_TagAndProbe_EFTriggerEfficiency_Eta.clear();
	//m_TagAndProbe_EFTriggerEfficiency_Phi.clear();
	//m_TagAndProbe_EFTriggerEfficiency_Pt.clear();
      }
    }
    /*
      m_TagAndProbe_Charge->Clear();
      m_TagAndProbe_Cut_Flow->Clear();
      m_TagAndProbe_ID_diMuonMass->Clear();
      m_TagAndProbe_ID_diMuonMass_AfterVtx->Clear();
      m_TagAndProbe_ID_diMuonMass_Refitted->Clear();
      m_TagAndProbe_ID_Selected_JpsiMass->Clear();
      m_TagAndProbe_ID_Selected_RefJpsiMass->Clear();
      m_TagAndProbe_ID_DeltaR->Clear();
      m_TagAndProbe_ID_TrackPt->Clear();
      m_TagAndProbe_ID_NCandidates->Clear();
      m_TagAndProbe_ID_Propertime->Clear();
      m_ID_MassShift_vs_Eta->Clear();
      m_ID_MassShift_vs_Phi->Clear();
      m_ID_MassShift_vs_Pt->Clear();
    */
    if (m_doTrigger) {
      if (m_trigEffJpsiTools->finalize().isFailure()) {
	ATH_MSG_WARNING("Failed to finalize TrigEffJpsiTool");
      }
    }
    /*
      ATH_MSG_DEBUG( "De-allocation of pointers.1" );
      m_TagAndProbe_Ntotal_vs_Eta->Delete();
      m_TagAndProbe_Ntotal_vs_Pt->Delete();
      m_TagAndProbe_Ntotal_vs_Phi->Delete();
      ATH_MSG_DEBUG( "De-allocation of pointers.2" );
      m_TagAndProbe_Ntotal_vs_Eta=0;
      m_TagAndProbe_Ntotal_vs_Pt=0;
      m_TagAndProbe_Ntotal_vs_Phi=0;
    */

  }  // isEndOfRun

  return StatusCode::SUCCESS;
} //=========================================end of procHistograms()======================================================
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
StatusCode JpsiMuonInsituPerformance::finalize()
{
  ATH_MSG_DEBUG("In finalize()");
  
  return StatusCode::SUCCESS;
}
//#######################################################################
bool JpsiMuonInsituPerformance::isAssociatedWithMSTrack(const Rec::TrackParticle *trk,std::string muonContainerName) 
{  
  /* 
     - loop on combined muon container; 
     - get the pointer from the combined muon to its inner detector track; 
     - check if it matches the probe ID-track; 
     - if yes, then the probe track is associated with a muon spectrometer track or low-pt muon track 
  */

  ATH_MSG_DEBUG( "in isAssociatedWithMStrack()" );

  const Analysis::MuonContainer* importedMuonContainer;
  if(m_storeGate->retrieve(importedMuonContainer, muonContainerName ).isFailure()){
    ATH_MSG_ERROR( "Couldn't retrieve Muon Container" << muonContainerName << " from StoreGate" );
    return StatusCode::FAILURE;
  }
  
  Analysis::MuonContainer::const_iterator muItr;
  for ( muItr=importedMuonContainer->begin(); muItr!=importedMuonContainer->end(); muItr++) {
    if ((*muItr)->hasInDetTrackParticle()) {
      if ( (*muItr)->inDetTrackParticle()==trk) {
	return true;
      }
    }
  }
  return false;
}
//======================================================================
bool JpsiMuonInsituPerformance::isElectron(const Rec::TrackParticle *track)
{
  StatusCode sc = StatusCode::SUCCESS;
 
  // retrieve the Electron container
  const ElectronContainer *elecTES = 0;
  sc = m_storeGate->retrieve(elecTES, "ElectronAODCollection");
  if( sc.isFailure() || !elecTES )  {
    ATH_MSG_ERROR( "Can't retrieve electron container from StoreGate." );
    return sc;
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

//============================= Vertexing methods =======================================

bool JpsiMuonInsituPerformance::doVertexing(const Rec::TrackParticle *theTag, const Rec::TrackParticle *theProbe, Trk::RecVertex &primaryVertex,double &vtxChi2, double &mass, double &tau)
{
  ATH_MSG_DEBUG( "...in doVertexing()" );
 
  std::vector<const Trk::TrackParticleBase*> theTracks; theTracks.clear();
  theTracks.push_back(static_cast<const Trk::TrackParticleBase*>(theTag));
  theTracks.push_back(static_cast<const Trk::TrackParticleBase*>(theProbe));

  // The muon masses vector
  std::vector<double> myMuonMasses;
  myMuonMasses.push_back(105.66);
  myMuonMasses.push_back(105.66);

  const Trk::Perigee * aPerigee1 = dynamic_cast<const Trk::Perigee*>(&(theTracks[0])->definingParameters());
  const Trk::Perigee * aPerigee2 = dynamic_cast<const Trk::Perigee*>(&(theTracks[1])->definingParameters());
  int sflag = 0;
  int errorcode = 0;
  Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(aPerigee1,aPerigee2,sflag,errorcode);
  if (errorcode != 0) {startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
  Trk::Vertex vertex(startingPoint);
  const Trk::ParametersBase<5,Trk::Charged>* track1 = m_helpertool->getTrkParticleParameters((theTracks[0]));
  const Trk::ParametersBase<5,Trk::Charged>* track2 = m_helpertool->getTrkParticleParameters((theTracks[1]));
  std::vector<const Trk::ParametersBase<5,Trk::Charged>*> vertexTracks; vertexTracks.clear();
  vertexTracks.push_back(track1);
  vertexTracks.push_back(track2);
  
  Trk::VxCandidate* myVxCandidate(0);
  myVxCandidate = m_iVertexFitter->fit(vertexTracks, vertex);
  
  // Check for good fit
  if( myVxCandidate ) {
    const Trk::ExtendedVxCandidate* myExVxCandidate = dynamic_cast<Trk::ExtendedVxCandidate* >(myVxCandidate);
    bool deleteVx = false;
    if (!myExVxCandidate) {
      myExVxCandidate = new Trk::ExtendedVxCandidate(myVxCandidate->recVertex(), *(myVxCandidate->vxTrackAtVertex()), 0);
      deleteVx = true;
    }

    vtxChi2 = 1.*m_V0Tools->chisq(&(*myExVxCandidate)) / m_V0Tools->ndof(&(*myExVxCandidate));
    //Lxy = m_V0Tools->lxy(&(*myExVxCandidate),(*primaryVertex));
    const double mytau = m_V0Tools->tau(&(*myExVxCandidate),primaryVertex,3096.9);
    const double mymass = m_V0Tools->invariantMass(&(*myExVxCandidate),myMuonMasses);
    tau = mytau;
    mass = mymass;

    if (deleteVx) delete myExVxCandidate;
  }
  else { 
    ATH_MSG_DEBUG( "Vertexing Failed..." );
    delete myVxCandidate;	  
    return false;
  }
  delete myVxCandidate; 
  return true;
}
/*
  double JpsimumuSelectionTool::CorrectInvMassTwoMuons(const Rec::TrackParticle *track1, const Rec::TrackParticle *track2)
  {  
  double e  =  sqrt( mMuon*mMuon + track1->e()*track1->e() - track1->hlv().m()* track1->hlv().m() ) ;
  e +=  sqrt( mMuon*mMuon + track2->e()*track2->e() - track2->hlv().m()* track2->hlv().m() ) ;  
  double px =  track1->px() + track2->px();
  double py =  track1->py() + track2->py();
  double pz =  track1->pz() + track2->pz();
    
  if((e*e-px*px-py*py-pz*pz) < 0 )m_log << MSG::ERROR << "Negative mass " << e*e-px*px-py*py-pz*pz << endreq  ;
   
  double mass = sqrt(fabs(e*e-px*px-py*py-pz*pz)) ;
    
  //m_log << MSG::DEBUG <<"M1, M2, M1+M2, M1+M2 corrected "<< track1->hlv().m()<<" "<<track2->hlv().m()<<" "<<(track1->hlv()+track2->hlv()).m()<<" "<<mass<<std::endl;
    
  return mass ;
  }
*/

void setDrawOptions(TH1F *h )
{
  //hTotal->Divide(hPassed);
  
  h->SetMarkerStyle(2);
  h->SetOption("P");
  h->GetYaxis()->SetRangeUser(0.,1.1);

  return;
}

bool JpsiMuonInsituPerformance::isGoodTrack(const Rec::TrackParticle *trk) 
{  
  ATH_MSG_DEBUG( "in isGoodTrack()" );
  
  const Trk::TrackSummary* trkSummary(0);
  trkSummary = trk->trackSummary();
  if (trkSummary==0) {
    ATH_MSG_DEBUG( "Could not create TrackSummary" );  
    return false;
  }
  const Trk::Perigee* perigee = trk->measuredPerigee();
  if (perigee==NULL) {
    ATH_MSG_DEBUG( "Could not get MeasuredPerigee" );  
    return false;
  }
  const Trk::FitQuality* quality = trk->fitQuality();
  if (quality==NULL) {
    ATH_MSG_DEBUG( "Could not get FitQuality" );  
    return false;
  }

  int nhitsTRT = trkSummary->get(Trk::numberOfTRTHits);
  int nOutliersTRT = trkSummary->get(Trk::numberOfTRTOutliers);
  int nhitsSCT = trkSummary->get(Trk::numberOfSCTHits);
  int nhitsPix = trkSummary->get(Trk::numberOfPixelHits); //including the b-layer    

  double p = 1./fabs(perigee->parameters()[Trk::qOverP]);
  double d0 = trk->measuredPerigee()->parameters()[Trk::d0];
  double d0Err = (*perigee->covariance())(Trk::d0,Trk::d0); 
  double d0Significance = d0Err==0 ? -1 : d0/d0Err;
  double ndf = trk->fitQuality()->doubleNumberDoF();
  double chi2ndf = ndf==0 ? -1 : trk->fitQuality()->chiSquared()/ndf;

  m_Probe_Nhits_vs_NoutliersTRT->Fill(nOutliersTRT,nhitsTRT);
  m_Probe_NhitsSCT->Fill(nhitsSCT);
  m_Probe_NhitsPix->Fill(nhitsPix);
  m_Probe_d0Sig->Fill(d0Significance);
  m_Probe_chi2ndf->Fill(chi2ndf);
  m_Probe_P->Fill(p);
    
  if (p>m_cut_p)
    if (nhitsTRT>m_cut_nhTRT)
      if (nOutliersTRT<m_cut_noTRT || m_cut_noTRT<=0)
	if (nhitsPix>m_cut_nhPix)
	  if (nhitsSCT>m_cut_nhSCT)
	    if (d0Significance>m_cut_d0Significance || m_cut_d0Significance==0)
	      if (chi2ndf<m_cut_chi2 || m_cut_chi2==0)
		return true;
  
  return false;
}
