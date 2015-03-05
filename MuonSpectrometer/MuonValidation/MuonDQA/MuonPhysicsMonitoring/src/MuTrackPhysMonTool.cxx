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

#include "MuonPhysicsMonitoring/MuTrackPhysMonTool.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

MuTrackPhysMonTool::MuTrackPhysMonTool( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator/TrackToVertexIPEstimator")
   //,m_muonIsolationTool("MuonIsolationTool/MuonIsolationTool")
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

  declareProperty( "muonDeltaPhiCut",	        m_cut_deltaPhi = 0);
  declareProperty( "diMuonMassWindow",	        m_cut_diMuonMass = 0);
  declareProperty( "tagPtCut",                  m_cut_tagPt = 0);
  declareProperty( "probePtCut" ,               m_cut_probePt = 0); //preselection pt-cut on probes
  declareProperty( "muonEtaCut",                m_cut_muonEta = 0); //preselection eta-cut on probes 
  declareProperty( "muonPairSign",              m_cut_charge=-1); //keep OS pairs

  declareProperty( "isolationJet",              m_cut_isolationJet=0.);
  declareProperty( "isolationTrkIso",           m_cut_isolationTrk=0.); //relative Track Isolation: Sum(pt)/pt
  declareProperty( "isolationET",               m_cut_isolationET=0.);
  
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

  //Trigger decision
  declareProperty( "useTrigger",                m_doTrigger);

  //tools
  declareProperty( "TrackToVertexIPEstimator",  m_trackToVertexIPEstimator, "tool for IP estimator");
  //declareProperty( "MuonIsolationTool",         m_muonIsolationTool );

  declareProperty( "binsEta",                   m_binsEta );
  declareProperty( "binsPhi",                   m_binsPhi );
  declareProperty( "binsPt",                    m_binsPt );
}

//======================================================================================//
/**  Initialize */
//======================================================================================//
StatusCode MuTrackPhysMonTool::initialize() 
{ 
  ATH_MSG_DEBUG( "initialize()" );
  StatusCode sc = ManagedMonitorToolBase::initialize(); 
  if(!sc.isSuccess()) return sc; 

  sc = StatusCode::SUCCESS;    
  /** Initialize tools and services */
  sc = service("StoreGateSvc", m_storeGate);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_FATAL( "Unable to retrieve pointer to StoreGateSvc" );
    return StatusCode::FAILURE;
  }

  /// get a handle on the analysis tools
  IToolSvc* toolSvc;
  sc = service("ToolSvc",toolSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_FATAL( " Can't get ToolSvc" );
    return StatusCode::FAILURE;
  }

  sc = service("THistSvc", m_thistSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_FATAL( "Unable to retrieve pointer to THistSvc" );
    return StatusCode::FAILURE;
  }

  if ( m_doTrigger ) {
    //m_all   = m_trigDecTool->getChainGroup(".*");
    //m_allL1 = m_trigDecTool->getChainGroup("L1_MU.*");
    //m_allL2 = m_trigDecTool->getChainGroup("L2_.*");
    //m_allEF = m_trigDecTool->getChainGroup("EF_.*");
  }  
 
  sc = m_trackToVertexIPEstimator.retrieve();
  if ( sc.isFailure() )  {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackToVertexIPEstimator );
    return StatusCode::FAILURE;
  }
  else ATH_MSG_INFO( "Retrieved tool " << m_trackToVertexIPEstimator );

#if 0
  // Muon Isolation tool
  if (m_muonIsolationTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Could not get " << m_muonIsolationTool.typeAndName() );
    return StatusCode::FAILURE;       
  }
#endif

  nEvents = 0;
  nZReco = 0;  
  nZMatch = 0;  
  nZTruth = 0;  

  return StatusCode::SUCCESS;
}

//=============================================================================================================
StatusCode MuTrackPhysMonTool::bookHistogramsRecurrent()
{
  ATH_MSG_DEBUG( "in bookHistograms()" );
  
  StatusCode sc = StatusCode::SUCCESS;
  
  if(newEventsBlock){}
  if(newLumiBlock){}
  if(newRun) {
   
    //////////////////////////////////////////////////////////////////
    std::string m_muonphysicsmonitoring_path = "Muon/MuonPhysicsMonitoring/Zmutrk/trkQuality/";
    MonGroup muonphysmon_tagandprobecut( this, m_muonphysicsmonitoring_path+"TagAndProbe_Cut_Histograms", run, ATTRIB_UNMANAGED );

    ///// Tag ////
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_TrackPt = TH1F_LW::create( "m_Tag_TrackPt", "Tag - Track Pt [MeV]", 100,0.,100000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_TrackEta = TH1F_LW::create( "m_Tag_TrackEta", "Tag - Track Eta", 54, -2.7, 2.7 ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_AbsCaloIso =TH1F_LW::create( "m_Tag_AbsCaloIso", "Tag - Abs Calo Isolation (E_{T}) [MeV]", 100,0.,50000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_RelCaloIso = TH1F_LW::create( "m_Tag_RelCaloIso", "Tag - Rel Calo Isolation (E_{T}/p_{T})", 100,0.,5. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_AbsJetIso = TH1F_LW::create( "m_Tag_AbsJetIso", "Tag - Abs Jet Isolation (p^{Jet}_{T}) [MeV]", 100,0.,50000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_RelJetIso = TH1F_LW::create( "m_Tag_RelJetIso", "Tag - Rel Jet Isolation (p^{Jet}_{T}/p_{T})", 100,0.,5. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_AbsTrkIso = TH1F_LW::create( "m_Tag_AbsTrkIso", "Tag - Abs Track Isolation (#Sigma p_{T} in cone/p_{T}) ", 100,0.,50000. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_RelTrkIso = TH1F_LW::create( "m_Tag_RelTrkIso", "Tag - Rel Track Isolation (#Sigma p_{T} in cone/p_{T}) ", 100,0.,5. ));
    sc=muonphysmon_tagandprobecut.regHist(m_Tag_NCandidates = TH1F_LW::create( "m_Tag_NCandidates", "Number of tags", 10,0,10 ));

    ////// Probe //////
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_TrackPt = TH1F_LW::create( "m_Probe_TrackPt", "Probe Track Pt [MeV]", 100,0.,100000.));
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_TrackEta = TH1F_LW::create( "m_Probe_TrackEta", "Probe Track Eta [MeV]", 54, -2.7, 2.7 )); 
    sc=muonphysmon_tagandprobecut.regHist( m_Probe_AbsCaloIso = TH1F_LW::create( "m_Probe_AbsCaloIso", "Probe - Abs Calo Isolation (E_{T}) [MeV]", 100,0.,50000. )); 
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_RelCaloIso = TH1F_LW::create( "m_Probe_RelCaloIso", "Probe - Rel Calo Isolation (E_{T}/p_{T})", 100,0.,5. )); 
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_AbsJetIso = TH1F_LW::create( "m_Probe_AbsJetIso", "Probe - Abs Jet Isolation (p^{Jet}_{T}) [MeV]", 100,0.,50000. )); 
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_RelJetIso = TH1F_LW::create( "m_Probe_RelJetIso", "Probe - Rel Jet Isolation (p^{Jet}_{T}/p_{T})", 100,0.,5. )); 
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_AbsTrkIso = TH1F_LW::create( "m_Probe_AbsTrkIso", "Probe - Abs Track Isolation (#Sigma p_{T} in cone/p_{T}) ", 100,0.,50000. ));     
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_RelTrkIso = TH1F_LW::create( "m_Probe_RelTrkIso", "Probe - Rel Track Isolation (#Sigma p_{T} in cone/p_{T}) ", 100,0.,5. )); 
    sc=muonphysmon_tagandprobecut.regHist(m_Probe_NCandidates = TH1F_LW::create( "m_Probe_NCandidates", "Number of probes", 10,0,10 ));

    ////// Tag & Probe pair /////////////// 
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_Cut_Flow = TH1F_LW::create( "m_TagAndProbe_Cut_Flow" , "TagAndProbe_Cut flow (1:#mu+trk, 2:Mass, 3:Charge, 4:#Delta #phi, 5:trk #mu)", 6, 0.5, 6.5)); 
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_diMuonMass = TH1F_LW::create( "m_TagAndProbe_diMuonMass", "Invariant mass [MeV]", 100,0.,120000. )); 
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ZMass = TH1F_LW::create( "m_TagAndProbe_ZMass", "TagAndProbe Z Invariant Mass [MeV]", 100,91200.-m_cut_diMuonMass,91200.+m_cut_diMuonMass )); 
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_Pt = TH1F_LW::create( "m_TagAndProbe_Pt", "Invariant p_{T} of ID track pairs", 100,0.,100000 )); 
    sc=muonphysmon_tagandprobecut.regHist( m_TagAndProbe_DeltaPhi = TH1F_LW::create( "m_TagAndProbe_DeltaPhi", "#Delta#phi of ID track pairs", 64,0.,3.2 )); 
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_NCandidates = TH1F_LW::create( "m_TagAndProbe_NCandidates", "Number of tag&probe pair candidates", 10,0,10 )); 
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_Charge = TH1F_LW::create( "m_TagAndProbe_Charge", "TagAndProbe Charge_{1}*Charge_{2}", 3,-1.5,1.5));

    //Reconstruction efficiencies
    MonGroup muonphysmon_efficiencies( this, m_muonphysicsmonitoring_path+"Efficiencies/", run, ATTRIB_UNMANAGED ); 
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Efficiency_Eta = new TH1F( "m_TagAndProbe_Efficiency_Eta" ,"TagAndProbe_Efficiency_Eta" , m_binsEta.size()-1 , &m_binsEta[0] ));
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Efficiency_Phi = new TH1F( "m_TagAndProbe_Efficiency_Phi" ,"TagAndProbe_Efficiency_Phi" , m_binsPhi.size()-1 , &m_binsPhi[0] )); 
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Efficiency_Pt = new TH1F( "m_TagAndProbe_Efficiency_Pt" ,"TagAndProbe_Efficiency_Pt" , m_binsPt.size()-1 , &m_binsPt[0] ));

    //..............Probe track muon
    sc=muonphysmon_tagandprobecut.regHist( m_ProbeTrk_Cut_Flow = TH1F_LW::create( "m_ProbeTrk_Cut_Flow" , "Probe Track - Cut Flow (1:nProbes, 2:p_{T}, 3:TrackIso, 4:nhSCT, 5:nhPIX, 6:d_{0}, 7:#chi^{2}/ndf)" , 8,0.5,8.5));
    sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_TrackPt = TH1F_LW::create( "m_ProbeTrk_TrackPt", "Probe Track - Track p_{T} [MeV]", 100,0,100000));
    //sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_TrackIso = TH1F_LW::create("m_ProbeTrk_TrackIso","Probe Track - Relative Track Isolation (#Sigma p_{T} in cone/p_{T})", 100, 0,5));
    sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_NhitsSCT = TH1F_LW::create("m_ProbeTrk_NhitsSCT", "Probe Track - Number of SCT hits", 20,0,20));
    sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_NhitsPIX = TH1F_LW::create("m_ProbeTrk_NhitsPIX", "Probe Track - Number of PIX hits", 20,0,20));
    sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_d0 = TH1F_LW::create("m_ProbeTrk_d0", "Probe Track - Impact Parameter d_{0} [mm]", 100, -5, 5));
    sc=muonphysmon_tagandprobecut.regHist(m_ProbeTrk_chi2ndf = TH1F_LW::create("m_ProbeTrk_chi2ndf", "Probe Track - #chi^{2}/ndf", 80, 0, 20));

    //Efficiencies truth
    MonGroup muonphysmon_efficiencies_truth( this, m_muonphysicsmonitoring_path+"Efficiencies/TrackMuon/Truth", run, ATTRIB_UNMANAGED );

    if (m_doTruth) { 
      sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Eta_Truth = new TH1F( "m_TagAndProbe_Efficiency_Eta_Truth" ,"TagAndProbe_Efficiency_Eta_Truth" , m_binsEta.size()-1 , &m_binsEta[0] )); 
      sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Phi_Truth = new TH1F( "m_TagAndProbe_Efficiency_Phi_Truth" ,"TagAndProbe_Efficiency_Phi_Truth" , m_binsPhi.size()-1 , &m_binsPhi[0] )); 
      sc=muonphysmon_efficiencies_truth.regHist( m_TagAndProbe_Efficiency_Pt_Truth = new TH1F( "m_TagAndProbe_Efficiency_Pt_Truth" ,"TagAndProbe_Efficiency_Pt_Truth" , m_binsPt.size()-1 , &m_binsPt[0] ));
      m_mutrk_pttruth  = TH1F_LW::create("m_mutrk_pttruth", "",m_binsPt.size()-1, &m_binsPt[0] );
      m_mutrk_etatruth = TH1F_LW::create("m_mutrk_etatruth", "",m_binsEta.size()-1, &m_binsEta[0] );
      m_mutrk_phitruth = TH1F_LW::create("m_mutrk_phitruth", "",m_binsPhi.size()-1, &m_binsPhi[0] );
    }

    if(sc.isFailure()) { 
      ATH_MSG_FATAL( "Failed to register histogram(s)" );   
      return StatusCode::FAILURE;
    }
    
    m_mutrk_pttot    = TH1F_LW::create("m_mutrk_pttot", "",m_binsPt.size()-1, &m_binsPt[0] );
    m_mutrk_etatot   = TH1F_LW::create("m_mutrk_etatot", "",m_binsEta.size()-1, &m_binsEta[0] );
    m_mutrk_phitot   = TH1F_LW::create("m_mutrk_phitot", "",m_binsPhi.size()-1, &m_binsPhi[0] );
    m_mutrk_ptpassed  = TH1F_LW::create("m_mutrk_ptpassed", "",m_binsPt.size()-1, &m_binsPt[0] );
    m_mutrk_etapassed = TH1F_LW::create("m_mutrk_etapassed", "",m_binsEta.size()-1, &m_binsEta[0] );
    m_mutrk_phipassed = TH1F_LW::create("m_mutrk_phipassed", "",m_binsPhi.size()-1, &m_binsPhi[0] );

    /*
      m_mutrk_pttrig = new TH1F("m_mutrk_pttrig", "",m_binsPt.size()-1, &m_binsPt[0] );
      sc=muonphysmon_efficiencies.regHist(m_mutrk_pttrig);
    
      m_mutrk_etatrig = new TH1F("m_mutrk_etatrig", "",m_binsEta.size()-1, &m_binsEta[0] );
      sc=muonphysmon_efficiencies.regHist(m_mutrk_etatrig);
    
      m_mutrk_phitrig = new TH1F("m_mutrk_phitrig", "",m_binsPhi.size()-1, &m_binsPhi[0] );
      sc=muonphysmon_efficiencies.regHist(m_mutrk_phitrig);
    */

    
  }//isNewRun

  return sc;
} //============end of bookHistograms()=======================================================
//////////////////////////////////////////////////////////////////////////////////////////////
StatusCode MuTrackPhysMonTool::fillHistograms()
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
      const Rec::TrackParticle *mutrk = (*muItr)->inDetTrackParticle();
      double pt=(*muItr)->pt();
      if (pt==0) continue;
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
	if ((*muItr)->eta() < m_cut_muonEta || m_cut_muonEta==0)
	  if (tagCaloIsolationET < m_cut_isolationET || m_cut_isolationET==0) 
	    if (tagJetIsolationPt < m_cut_isolationJet || m_cut_isolationJet==0)
	      if ( tagTrkIsolationPt_rel < m_cut_isolationTrk || m_cut_isolationTrk==0)
		if (!isElectron(mutrk)) 
		  myTagMuons.push_back((*muItr)->inDetTrackParticle());
    }
  }
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
  const Rec::TrackParticleContainer *importedIDTrackContainer;
  sc = m_storeGate->retrieve(importedIDTrackContainer, m_InnerTrackContainer);  
  if( sc.isFailure() || importedIDTrackContainer == 0) {
    ATH_MSG_WARNING( "No ID Track Collection found in Storegate. Skipping event..." );
    return  StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Retrieved "<< m_InnerTrackContainer );
  ATH_MSG_DEBUG( "Found "<< importedIDTrackContainer->size() << " ID tracks in container" );

  std::vector<const Rec::TrackParticle *> myProbeTracks;

  Rec::TrackParticleContainer::const_iterator trkItr;
  for (trkItr=importedIDTrackContainer->begin(); trkItr!=importedIDTrackContainer->end(); trkItr++) {
    m_Probe_TrackPt->Fill((*trkItr)->pt());
    m_Probe_TrackEta->Fill((*trkItr)->eta());
    if ( (*trkItr)->pt() > m_cut_probePt ) 
      if ( (*trkItr)->eta() < m_cut_muonEta ) 
	if (!isElectron((*trkItr))) 
	  myProbeTracks.push_back((*trkItr));
  }
  m_Probe_NCandidates->Fill(myProbeTracks.size());
  if (myProbeTracks.size()==0) {
    ATH_MSG_DEBUG( "No probe tracks found, skipping event..." );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Got " << myProbeTracks.size() << " probes" );
  
  //apply event selection-----------------------------------
  int    nZ=0;
  //double pt1=0.;
  double pt2=0.;
  double phi1=0.;
  double phi2=0.;
  double charge1=0.;
  double charge2=0.;
  
  double probePtMax=0;
  //int    probePtMaxId=0;
  
  std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > myZmuons;
  for (unsigned int i=0; i<myTagMuons.size(); i++) {
    const Rec::TrackParticle *track1 = myTagMuons[i];
    charge1 = track1->charge();
    //pt1 = track1->pt();
    phi1 = track1->phi();

    for (unsigned int j=0; j<myProbeTracks.size(); j++) {
      const Rec::TrackParticle *track2 = myProbeTracks[j];
      if (track1==track2) continue;       
      charge2 = track2->charge();
      pt2 = track2->pt();
      phi2 = track2->phi();

      double pt = ( track1->hlv() + track2->hlv() ).perp();
      double deltaPhi=fabs(phi1-phi2);
      if (deltaPhi>3.14159) deltaPhi=6.28318-deltaPhi; 
      double invMass = ( track1->hlv() + track2->hlv() ).m(); 
      double charge = charge1*charge2;

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
	    nZ++;
	    m_TagAndProbe_ZMass->Fill(invMass);
	    myZmuons.push_back(std::make_pair(track1,track2));
	    if (pt2>probePtMax) { 
	      probePtMax=pt2;
	      //probePtMaxId=myZmuons.size()-1;
	    }
	  }
	}
      }
    }
  }//close for
  
  m_TagAndProbe_NCandidates->Fill(nZ);
  if (myZmuons.size()==0) {
    ATH_MSG_DEBUG( "No Z candidates" );
    return StatusCode::SUCCESS;
  }  
  ATH_MSG_INFO( "Found " << nZ << " Z candidates" );
  nZReco +=nZ;
  
  if (nZ>1) {
    //ATH_MSG_INFO( "Keeping only candidate with higher-pt probe" );
    //for (unsigned int i=
  }
 
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
  
  //Probing track2 in Muon spectrometer...
  for (unsigned int i=0; i<myZmuons.size(); i++) {
    //    const Rec::TrackParticle *tag = myZmuons[i].first;
    const Rec::TrackParticle *probe = myZmuons[i].second;
    
    //all probe muons
    m_mutrk_etatot->Fill(probe->eta(), 1.);
    m_mutrk_pttot->Fill(probe->pt(), 1.);
    m_mutrk_phitot->Fill(probe->phi(), 1.);
    
    if (isIDMuonTrack(probe)) {
      ATH_MSG_DEBUG( "Probe track passed ID muon track criteria");
      m_mutrk_etapassed->Fill(probe->eta() , 1.);
      m_mutrk_ptpassed->Fill(probe->pt() , 1.);
      m_mutrk_phipassed->Fill(probe->phi(), 1.);   
      m_TagAndProbe_Cut_Flow->Fill(5);
      /*
	if (m_doTruth && isTrueZ[i]) {
	m_mutrk_etatruth->Fill(probe->eta(), 1.);
	m_mutrk_pttruth->Fill(probe->pt(), 1.);
	m_mutrk_phitruth->Fill(probe->phi(), 1.);
	}
      */
    }
    else ATH_MSG_DEBUG( "Probe track didn't pass ID muon track criteria");
  }
  
  return StatusCode::SUCCESS;
} //=============end of fillHistograms()======================================================
//////////////////////////////////////////////////////////////////////////////////////////////
StatusCode MuTrackPhysMonTool::procHistograms()
{
  ATH_MSG_DEBUG( "in procHistograms()" );

  if(endOfEventsBlock){}
  if(endOfLumiBlock){}

  if(endOfRun){
    for (unsigned int j=1; j<=m_binsEta.size(); j++)
      if (m_mutrk_etatot->GetBinContent(j)>0) {
	m_TagAndProbe_Efficiency_Eta->Fill(m_binsEta[j-1], m_mutrk_etapassed->GetBinContent(j) / m_mutrk_etatot->GetBinContent(j));
      }
    m_TagAndProbe_Efficiency_Eta->SetMarkerStyle(2);
    m_TagAndProbe_Efficiency_Eta->SetOption("P");
    m_TagAndProbe_Efficiency_Eta->GetYaxis()->SetRangeUser(0.,1.1);
      
    for (unsigned int j=1; j<=m_binsPhi.size(); j++)
      if (m_mutrk_phitot->GetBinContent(j)>0) {
	m_TagAndProbe_Efficiency_Phi->Fill(m_binsPhi[j-1], m_mutrk_phipassed->GetBinContent(j) / m_mutrk_phitot->GetBinContent(j));
      }
    m_TagAndProbe_Efficiency_Phi->SetMarkerStyle(2);
    m_TagAndProbe_Efficiency_Phi->SetOption("P");
    m_TagAndProbe_Efficiency_Phi->GetYaxis()->SetRangeUser(0.,1.1);

    for (unsigned int j=1; j<=m_binsPt.size(); j++) 
      if (m_mutrk_pttot->GetBinContent(j)>0) {
	m_TagAndProbe_Efficiency_Pt->Fill(m_binsPt[j-1], m_mutrk_ptpassed->GetBinContent(j) / m_mutrk_pttot->GetBinContent(j));
      }
    m_TagAndProbe_Efficiency_Pt->SetMarkerStyle(2);
    m_TagAndProbe_Efficiency_Pt->SetOption("P");
    m_TagAndProbe_Efficiency_Pt->GetYaxis()->SetRangeUser(0.,1.1);

    if (m_doTruth) {
      for (unsigned int j=1; j<=m_binsEta.size(); j++)
	if (m_mutrk_etatot->GetBinContent(j)>0) {
	  m_TagAndProbe_Efficiency_Eta_Truth->Fill(m_binsEta[j-1], m_mutrk_etatruth->GetBinContent(j) / m_mutrk_etatot->GetBinContent(j));
	}
      m_TagAndProbe_Efficiency_Eta_Truth->SetMarkerStyle(3);
      m_TagAndProbe_Efficiency_Eta_Truth->SetMarkerColor(kRed);
      m_TagAndProbe_Efficiency_Eta_Truth->SetOption("P");
      m_TagAndProbe_Efficiency_Eta_Truth->GetYaxis()->SetRangeUser(0.,1.1);
      
      for (unsigned int j=1; j<=m_binsPhi.size(); j++)
	if (m_mutrk_phitot->GetBinContent(j)>0) {
	  m_TagAndProbe_Efficiency_Phi_Truth->Fill(m_binsPhi[j-1], m_mutrk_phitruth->GetBinContent(j) / m_mutrk_phitot->GetBinContent(j));
	}
      m_TagAndProbe_Efficiency_Phi_Truth->SetMarkerStyle(3);
      m_TagAndProbe_Efficiency_Phi_Truth->SetMarkerColor(kRed);
      m_TagAndProbe_Efficiency_Phi_Truth->SetOption("P");
      m_TagAndProbe_Efficiency_Phi_Truth->GetYaxis()->SetRangeUser(0.,1.1);

      for (unsigned int j=1; j<=m_binsPt.size(); j++) 
	if (m_mutrk_pttot->GetBinContent(j)>0) {
	  m_TagAndProbe_Efficiency_Pt_Truth->Fill(m_binsPt[j-1], m_mutrk_pttruth->GetBinContent(j) / m_mutrk_pttot->GetBinContent(j));
	}
      m_TagAndProbe_Efficiency_Pt_Truth->SetMarkerStyle(3);
      m_TagAndProbe_Efficiency_Pt_Truth->SetMarkerColor(kRed);
      m_TagAndProbe_Efficiency_Pt_Truth->SetOption("P");
      m_TagAndProbe_Efficiency_Pt_Truth->GetYaxis()->SetRangeUser(0.,1.1);
    
      // m_mutrk_pttruth->Delete();
      // m_mutrk_etatruth->Delete();
      // m_mutrk_phitruth->Delete();
      
      m_mutrk_pttruth=0;
      m_mutrk_etatruth=0;
      m_mutrk_phitruth=0;  
    }
    LWHist::safeDelete(m_mutrk_pttot);
    LWHist::safeDelete(m_mutrk_phitot);
    LWHist::safeDelete(m_mutrk_etatot);
    LWHist::safeDelete(m_mutrk_ptpassed);
    LWHist::safeDelete(m_mutrk_phipassed);
    LWHist::safeDelete(m_mutrk_etapassed);
    
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
} //=============end of procHistograms()======================================================
//////////////////////////////////////////////////////////////////////////////////////////////
StatusCode MuTrackPhysMonTool::finalize()
{
  return StatusCode::SUCCESS;
}
//---------------------------------------------------
bool MuTrackPhysMonTool::isIDMuonTrack(const Rec::TrackParticle *trk) 
{  
  ATH_MSG_DEBUG( "in isIDMuonTrack()" );
  
  ATH_MSG_DEBUG( "Retrieving TrackSummary Information" );
  const Trk::TrackSummary* trkSummary(0);
  trkSummary = trk->trackSummary();
  if (trkSummary==0) {
    ATH_MSG_INFO( "Could not create TrackSummary - Track will likely fail hits requirements" );  
    return false;
  }

  int nhitsSCT = trkSummary->get(Trk::numberOfSCTHits);
  int nhitsPixel = trkSummary->get(Trk::numberOfPixelHits); //including the b-layer    
  double pt=trk->pt();

 // double trkIso = m_muonIsolationTool->trackIsolationEnergy(trk, m_trk_coneIso);
 // double trkIso_rel=trkIso/pt;
  double d0 = getImpactParameter(trk);
  double ndf = trk->fitQuality()->doubleNumberDoF();
  if (ndf<=0) {
    ATH_MSG_INFO( "Skiping event while encountering track with " << ndf << " degrees of freedom" );
    return false;
  }
  double chi2ndf = trk->fitQuality()->chiSquared() / ndf;
  
  m_ProbeTrk_TrackPt->Fill(pt);
  // m_ProbeTrk_TrackIso->Fill(trkIso);
  m_ProbeTrk_NhitsSCT->Fill(nhitsSCT);
  m_ProbeTrk_NhitsPIX->Fill(nhitsPixel);
  m_ProbeTrk_d0->Fill(d0);
  m_ProbeTrk_chi2ndf->Fill(chi2ndf);
 
  m_ProbeTrk_Cut_Flow->Fill(1);
  if (pt > m_trk_pt ) {
    m_ProbeTrk_Cut_Flow->Fill(2);
   // if (trkIso_rel < m_trk_relTrkIso || m_trk_relTrkIso==0) {
     m_ProbeTrk_Cut_Flow->Fill(3);
      if (nhitsSCT >= m_trk_nhitsSCT) {
	m_ProbeTrk_Cut_Flow->Fill(4);    
	if (nhitsPixel >= m_trk_nhitsPix){
	  m_ProbeTrk_Cut_Flow->Fill(5);
	  if (d0 < m_trk_d0 || m_trk_d0==0) {
	    m_ProbeTrk_Cut_Flow->Fill(6);
	    if (chi2ndf < m_trk_chi2ndf || m_trk_chi2ndf==0) {
	      m_ProbeTrk_Cut_Flow->Fill(7);
	      return true;
	    }
	  }
	}
      }
 //   }
  }

  return false;
}
//======================================================================
bool MuTrackPhysMonTool::isElectron(const Rec::TrackParticle *track)const
{
    
  if(!evtStore()->contains<ElectronContainer>(m_ElectronContainer) ) return -1;
  // retrieve the Electron container
  const ElectronContainer *elecTES = 0;
  if( evtStore()->retrieve(elecTES,m_ElectronContainer).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve electron container with key "+m_ElectronContainer);
    return -1;
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

double MuTrackPhysMonTool::getTrackIsolationInCone(const Rec::TrackParticle *track, double coneMax, double coneMin)const
{
  ATH_MSG_DEBUG( "in getTrackIsolationInCone()");
  //return relative track isolation: sum of pt inside a delta-R cone around the track / pt of track
 
  if(!evtStore()->contains<Rec::TrackParticleContainer>(m_InnerTrackContainer) ) return -1;
  
  const Rec::TrackParticleContainer* importedInDetTrackContainer=0;
  
  if(evtStore()->retrieve(importedInDetTrackContainer, m_InnerTrackContainer).isFailure())  {
    ATH_MSG_WARNING( "Can't retrieve TrackParticle container from StoreGate." );
    return -1;
  }

  Rec::TrackParticleContainer::const_iterator trkItr   = importedInDetTrackContainer->begin();
  Rec::TrackParticleContainer::const_iterator trkItrE  = importedInDetTrackContainer->end();

  double sumPt=0.;
  for(; trkItr != trkItrE; trkItr++) {
    if ( fabs(track->eta()-(*trkItr)->eta())<10e-6 && fabs((*trkItr)->pt()-track->pt())<10e-6 ) { 
      ATH_MSG_DEBUG( "Not counting the ID track itself inside the cone" ); 
      continue;
    }
    
    double dR = sqrt( pow(track->eta()-(*trkItr)->eta(),2) + pow(track->phi()-(*trkItr)->phi(),2) );
    if ( dR>coneMin && dR<coneMax ) {
      sumPt+=(*trkItr)->pt();
    }
  }
  if (track->pt()==0) {
    ATH_MSG_WARNING( "Encountered track with pt = "<<track->pt()<<" !!! Can't calculate relative track isolation" );
    return -1;     
  }
  return sumPt/(track->pt());
}

double MuTrackPhysMonTool::getJetEnergyInCone(const Rec::TrackParticle *track) const 
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
 
double MuTrackPhysMonTool::getETIsolation(const Analysis::Muon *muon)
{
  ATH_MSG_DEBUG( "in getETIsolation()");
  return muon->parameter( MuonParameters::etcone40 );
}

double MuTrackPhysMonTool::getImpactParameter(const Rec::TrackParticle *track)const 
{
  ATH_MSG_DEBUG( "in getImpactParameter()" );

  if(!evtStore()->contains<VxContainer>("VxPrimaryCandidate") ) return -1;

  // Get primary vertex from StoreGate
  const VxContainer* importedVxContainer=0; //Points to primary vertices
  if( evtStore()->retrieve(importedVxContainer,"VxPrimaryCandidate").isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve Jet container with key VxPrimaryCandidate");
    return -1;
  }  
   
  const Trk::VxCandidate *myVxPrimaryCandidate = *(importedVxContainer->begin());
  if (!myVxPrimaryCandidate) {
    ATH_MSG_WARNING( "No Primary Vertex found in container, cannot estimate d0" );
    return 999999; 
  }
  ATH_MSG_DEBUG( "Retrieving impact parameter with TrackToVertexIPEstimator" );
  const Trk::ImpactParametersAndSigma *myIPS = m_trackToVertexIPEstimator->estimate( track, myVxPrimaryCandidate, true);
  if (!myIPS) {
    ATH_MSG_INFO("d0 calculation failed");
    return 999999;
  }
  double d0wrtPvx = myIPS->IPd0;
  //  double d0ErrwrtPvx = myIPS->sigmad0;
  ATH_MSG_DEBUG( "Deleting ImpactParametersAndSigma object" );
  delete myIPS;
  
  return d0wrtPvx;
}
