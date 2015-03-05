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

#include "MuonPhysicsMonitoring/MuonInsituPerformance.h"

MuonInsituPerformance::MuonInsituPerformance( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent )
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
  declareProperty( "tagMuonPtCut",              m_cut_tagMuonPt = 0);
  declareProperty( "probeMuonPtCut" ,           m_cut_probeMuonPt = 0); //preselection pt-cut on muons
  declareProperty( "muonEtaCut",                m_cut_muonEta = 0); //preselection eta-cut on muons 

  declareProperty( "isolationJet",              m_cut_isolationJet=0.);
  declareProperty( "isolationNtracks",          m_cut_isolationNtracks=0.);
  declareProperty( "isolationPtSum",            m_cut_isolationPtSum=0.);
  declareProperty( "isolationET",               m_cut_isolationET=0.);
  //isolation
  declareProperty( "isoConeMax",                m_isoConeMax=0.);
  declareProperty( "isoConeMin",                m_isoConeMin = 0);
  declareProperty( "binsEta",                   m_binsEta);
  declareProperty( "binsPhi",                   m_binsPhi );
  declareProperty( "binsPt",                    m_binsPt );
  //Trigger decision
  declareProperty( "useTrigger",                m_doTrigger);

}

//======================================================================================//
/**  Initialize */
//======================================================================================//
StatusCode MuonInsituPerformance::initialize() 
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

  sc = service("THistSvc", m_thistSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR( "Unable to retrieve pointer to THistSvc" );
    return StatusCode::FAILURE;
  }

  if ( m_doTrigger ) {
    //m_all   = m_trigDecTool->getChainGroup(".*");
    //m_allL1 = m_trigDecTool->getChainGroup("L1_MU.*");
    //m_allL2 = m_trigDecTool->getChainGroup("L2_.*");
    //m_allEF = m_trigDecTool->getChainGroup("EF_.*");
  }  

  nEvents = 0;
  nZReco = 0;  
  nZMatch = 0;  
  nZTruth = 0;  
  
  return StatusCode::SUCCESS;
}

//=============================================================================================================
StatusCode MuonInsituPerformance::bookHistogramsRecurrent()
{
  ATH_MSG_DEBUG( "in bookHistograms()" );  
  
  if(newEventsBlock){}
  if(newLumiBlock){}
  if(newRun) {
   
    StatusCode sc = StatusCode::SUCCESS;
    //////////////////////////////////////////////////////////////////
    std::string m_muonphysicsmonitoring_path = "Muon/MuonPhysicsMonitoring/Zmumu/";
    MonGroup muonphysmon_tagandprobecut( this, m_muonphysicsmonitoring_path+"TagAndProbe_Cut_Histograms/"+m_MuonContainer, run, ATTRIB_UNMANAGED );
    
    m_TagAndProbe_Cut_Flow = new TH1F( "m_TagAndProbe_Cut_Flow" , "TagAndProbe_Cut flow (1:#mu^{+}#mu^{-}, 2:Mass, 3:Isolation (Jet,N_{trk},p_{T}), 4:!electron, 5:#Delta #phi, 6:ProbeAssociation )" , 7,0.5,7.5);
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_Cut_Flow);
    
    m_TagAndProbe_ID_diMuonMass = new TH1F( "m_TagAndProbe_ID_diMuonMass", "Di-muon invariant mass [MeV]", 100,0.,120000. );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_diMuonMass);
    
    m_TagAndProbe_ID_Selected_ZMass = new TH1F( "m_TagAndProbe_ID_Selected_ZMass", "Selected Z mass [MeV]", 100,91000.-m_cut_diMuonMass,91000.+m_cut_diMuonMass );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_Selected_ZMass);

    m_TagAndProbe_ID_DeltaPhi = new TH1F( "m_TagAndProbe_ID_DeltaPhi", "#Delta#phi of ID track pairs", 32,0.,3.2 );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_DeltaPhi);

    m_TagAndProbe_ID_TrackPt = new TH1F( "m_TagAndProbe_ID_TrackPt", "Track Pt [MeV]", 100,0.,100000. );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_TrackPt);

    m_TagAndProbe_ID_NCandidates = new TH1F( "m_TagAndProbe_ID_NCandidates", "TagAndProbe Number of Z Candidate Pairs of ID tracks", 10,0,10 );
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_ID_NCandidates);

    m_TagAndProbe_Charge = new TH1F( "m_TagAndProbe_Charge", "TagAndProbe Charge_{1}*Charge_{2}", 3,-1.5,1.5);
    sc=muonphysmon_tagandprobecut.regHist(m_TagAndProbe_Charge);

    m_ID_MassShift_vs_Pt = new TProfile("m_ID_MassShift_vs_Pt","Reconstructed ID Z mass shift vs p_{T}",m_binsPt.size()-1 , &m_binsPt[0] ) ;
    sc=muonphysmon_tagandprobecut.regHist(m_ID_MassShift_vs_Pt );

    m_ID_MassShift_vs_Eta = new TProfile("m_ID_MassShift_vs_Eta","Reconstructed ID Z mass shift vs #eta", m_binsEta.size()-1 , &m_binsEta[0] ) ;
    sc=muonphysmon_tagandprobecut.regHist( m_ID_MassShift_vs_Eta);

    m_ID_MassShift_vs_Phi = new TProfile("m_ID_MassShift_vs_Phi","Reconstructed ID Z mass shift vs #phi", m_binsPhi.size()-1 , &m_binsPhi[0] ) ;
    sc=muonphysmon_tagandprobecut.regHist(m_ID_MassShift_vs_Phi );
   
    //Reconstruction efficiencies
    MonGroup muonphysmon_efficiencies( this, m_muonphysicsmonitoring_path+"Efficiencies/MuonReconstruction/"+m_MuonContainer, run, ATTRIB_UNMANAGED );

    m_TagAndProbe_Efficiency_Eta = new TH1F( "m_TagAndProbe_Efficiency_Eta" ,"TagAndProbe_Efficiency_Eta" , m_binsEta.size()-1 , &m_binsEta[0] );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Efficiency_Eta);

    m_TagAndProbe_Efficiency_Phi = new TH1F( "m_TagAndProbe_Efficiency_Phi" ,"TagAndProbe_Efficiency_Phi" , m_binsPhi.size()-1 , &m_binsPhi[0] );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Efficiency_Phi);

    m_TagAndProbe_Efficiency_Pt = new TH1F( "m_TagAndProbe_Efficiency_Pt" ,"TagAndProbe_Efficiency_Pt" , m_binsPt.size()-1 , &m_binsPt[0] );
    sc=muonphysmon_efficiencies.regHist(m_TagAndProbe_Efficiency_Pt);

    //Efficiencies truth
    MonGroup muonphysmon_efficiencies_truth( this, m_muonphysicsmonitoring_path+"Efficiencies/MuonReconstruction/Truth/"+m_MuonContainer, run, ATTRIB_UNMANAGED );

    if (m_doTruth) {
      m_TagAndProbe_Efficiency_Eta_Truth = new TH1F( "m_TagAndProbe_Efficiency_Eta_Truth" ,"TagAndProbe_Efficiency_Eta_Truth" , m_binsEta.size()-1 , &m_binsEta[0] );
      sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Eta_Truth);
      
      m_TagAndProbe_Efficiency_Phi_Truth = new TH1F( "m_TagAndProbe_Efficiency_Phi_Truth" ,"TagAndProbe_Efficiency_Phi_Truth" , m_binsPhi.size()-1 , &m_binsPhi[0] );
      sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Phi_Truth);
      
      m_TagAndProbe_Efficiency_Pt_Truth = new TH1F( "m_TagAndProbe_Efficiency_Pt_Truth" ,"TagAndProbe_Efficiency_Pt_Truth" , m_binsPt.size()-1 , &m_binsPt[0] );
      sc=muonphysmon_efficiencies_truth.regHist(m_TagAndProbe_Efficiency_Pt_Truth);

      m_h_pt_truth    = new TH1F("m_h_pt_truth", "",m_binsPt.size()-1, &m_binsPt[0] );
      m_h_eta_truth   = new TH1F("m_h_eta_truth", "",m_binsEta.size()-1, &m_binsEta[0] );
      m_h_phi_truth   = new TH1F("m_h_phi_truth", "",m_binsPhi.size()-1, &m_binsPhi[0] );
    }

    MonGroup muonphysmon_reco( this, m_muonphysicsmonitoring_path+"MuonReconstruction/"+m_MuonContainer, run, ATTRIB_UNMANAGED );

    m_h_pt_tot    = new TH1F("m_h_pt_tot", "pt_tot",m_binsPt.size()-1, &m_binsPt[0] );
    sc=muonphysmon_reco.regHist(m_h_pt_tot);
    
    m_h_eta_tot   = new TH1F("m_h_eta_tot", "eta_tot",m_binsEta.size()-1, &m_binsEta[0] );
    sc=muonphysmon_reco.regHist(m_h_eta_tot);
    
    m_h_phi_tot   = new TH1F("m_h_phi_tot", "phi_tot",m_binsPhi.size()-1, &m_binsPhi[0] );
    sc=muonphysmon_reco.regHist(m_h_phi_tot);
    
    m_h_pt_recon  = new TH1F("m_h_pt_recon", "pt_recon",m_binsPt.size()-1, &m_binsPt[0] );
    sc=muonphysmon_reco.regHist(m_h_pt_recon);
    
    m_h_eta_recon = new TH1F("m_h_eta_recon", "eta_recon",m_binsEta.size()-1, &m_binsEta[0] );
    sc=muonphysmon_reco.regHist(m_h_eta_recon);
    
    m_h_phi_recon = new TH1F("m_h_phi_recon", "phi_recon", m_binsPhi.size()-1, &m_binsPhi[0] );
    sc=muonphysmon_reco.regHist(m_h_phi_recon);
    
    /*
      m_h_pttrig = new TH1F("m_h_pttrig", "",m_binsPt.size()-1, &m_binsPt[0] );
      sc=muonphysmon_efficiencies.regHist(m_h_pttrig);
    
      m_h_etatrig = new TH1F("m_h_etatrig", "",m_binsEta.size()-1, &m_binsEta[0] );
      sc=muonphysmon_efficiencies.regHist(m_h_etatrig);
    
      m_h_phitrig = new TH1F("m_h_phitrig", "",m_binsPhi.size()-1, &m_binsPhi[0] );
      sc=muonphysmon_efficiencies.regHist(m_h_phitrig);
    */

    if(sc.isFailure()) { 
      ATH_MSG_FATAL( "Failed to register histogram(s)" );   
      return sc;
    }
    
  }//isNewRun

  return StatusCode::SUCCESS;
} //============end of bookHistograms()=======================================================
//////////////////////////////////////////////////////////////////////////////////////////////
StatusCode MuonInsituPerformance::fillHistograms()
{
  ATH_MSG_DEBUG( "in fillHistograms()" ); 

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
    if( (*muItr)->isCombinedMuon() ) {
      if ( (*muItr)->pt()>m_cut_tagMuonPt ) {
	if ( (*muItr)->eta()<m_cut_muonEta ) {
	  if ((*muItr)->hasInDetTrackParticle()) {
	    const Rec::TrackParticle *mutrk = (*muItr)->inDetTrackParticle();
	    double tagIsolationET = getETIsolation((*muItr));
	    double tagIsolationJet = getJetEnergyInCone(mutrk);
	    std::pair<double,double> tagPropertiesInCone = getTrackPropertiesInCone(mutrk);
	    double tagIsolationNtracks = tagPropertiesInCone.first;
	    double tagIsolationPtSum = tagPropertiesInCone.second;	    
	    if (tagIsolationET < m_cut_isolationET)
	      if (tagIsolationJet < m_cut_isolationJet)
		if (tagIsolationNtracks < m_cut_isolationNtracks)
		  if (tagIsolationPtSum < m_cut_isolationPtSum)
		    myTagMuons.push_back((*muItr)->inDetTrackParticle());	    
	  }
	}
      }
    }
  }
  ATH_MSG_DEBUG( "Got "<< myTagMuons.size() << " tag muons after preselection" );
  if (myTagMuons.size()==0) {
    ATH_MSG_DEBUG( "Can't find good tag muons. Skipping event..." );
    return sc;
  }
  //------------------------------------------------------------------------------
  //=====================================================
  // Selection of probe muons from inner detector tracks
  //=====================================================
  const Rec::TrackParticleContainer *importedIDTrackContainer;
  sc = m_storeGate->retrieve(importedIDTrackContainer, m_InnerTrackContainer);  
  if( sc.isFailure() || importedIDTrackContainer == 0) {
    ATH_MSG_ERROR( "No ID Track Collection found in Storegate. Skipping event..." );
    return sc;
  }
  ATH_MSG_DEBUG( "Retrieved "<< m_InnerTrackContainer );
  ATH_MSG_DEBUG( "Found "<< importedIDTrackContainer->size() << " ID tracks in container" );

  std::vector<const Rec::TrackParticle *> myIDTracks;
  Rec::TrackParticleContainer::const_iterator trkItr;
  for (trkItr=importedIDTrackContainer->begin(); trkItr!=importedIDTrackContainer->end(); trkItr++) {
    if ( (*trkItr)->pt() > m_cut_probeMuonPt )
      if ( (*trkItr)->eta() < m_cut_muonEta ) {	    
	myIDTracks.push_back((*trkItr));
	m_TagAndProbe_ID_TrackPt->Fill((*trkItr)->pt());
      }
  }
  ATH_MSG_DEBUG( "Got " << myIDTracks.size() << " ID tracks after preselection" );

  //Truth?
  //...
  //

  //Make pairs of each tag muon with oppositely charged inner detector tracks
  double charge1=0.;
  double charge2=0.;
  std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > myPairs;
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
    }
  }
  ATH_MSG_DEBUG( "Found " << myPairs.size() << " pairs" );
  if (myPairs.size()==0) {
    ATH_MSG_DEBUG( "No pairs, skipping event..." );
    return sc;
  }
  
  //select Z candidates
  int nZ=0;
  // double pt=0.;  
  // double pt1=0.;
  double pt2=0.;
   
  std::vector< std::pair<const Rec::TrackParticle *,const Rec::TrackParticle *> > myZMuons;
  
  for (unsigned int i=0; i<myPairs.size(); i++) {
    ATH_MSG_DEBUG( "Getting pair #"<< (i+1) );
    
    const Rec::TrackParticle *track1 = myPairs[i].first;
    const Rec::TrackParticle *track2 = myPairs[i].second;
    
    //  pt1 = track1->pt();
    pt2 = track2->pt();
    // pt = ( track1->hlv() + track2->hlv() ).perp();
    //    double eta1 = track1->eta();     double eta2 = track2->eta();
    double phi1 = track1->phi();
    double phi2 = track2->phi();
    double deltaPhi=fabs(phi1-phi2);
    if (deltaPhi>3.14159) deltaPhi=6.28318-deltaPhi; 
    double invMass = ( track1->hlv() + track2->hlv() ).m();
    
    double probeIsolationJet = getJetEnergyInCone(track2);
    std::pair<double,double> probePropertiesInCone = getTrackPropertiesInCone(track2);
    double probeIsolationNtracks = probePropertiesInCone.first;
    double probeIsolationPtSum = probePropertiesInCone.second;
    //    double probeIsolationET = getETIsolation(track);
   
    //Keep quantities before cuts
    m_TagAndProbe_ID_diMuonMass->Fill(invMass);
    m_TagAndProbe_ID_DeltaPhi->Fill(deltaPhi);
    //    m_TagAndProbe_PtMax->Fill(pt1);
    //    m_TagAndProbe_PtMin->Fill(pt2);
    //    m_TagAndProbe_diMuonPt->Fill(pt);
    
    m_TagAndProbe_ID_TrackPt->Fill(pt2); //only inner detector tracks (probe candidates)...

    //fill histograms for muon pairs, before cuts
    m_TagAndProbe_Cut_Flow->Fill(1);
    
    //apply cuts
    ATH_MSG_DEBUG( "Applying cuts..." );
    if ( fabs(invMass-91000.) < m_cut_diMuonMass ) {
      m_TagAndProbe_Cut_Flow->Fill(2);
      if ( probeIsolationJet < m_cut_isolationJet 
	   && probeIsolationNtracks < m_cut_isolationNtracks
	   && probeIsolationPtSum < m_cut_isolationPtSum) { 
	m_TagAndProbe_Cut_Flow->Fill(3);
	if (!isElectron(track1) && !isElectron(track2) ) {
	  m_TagAndProbe_Cut_Flow->Fill(4);
	  if (deltaPhi>m_cut_deltaPhi) {
	    m_TagAndProbe_Cut_Flow->Fill(5);	  
	    ATH_MSG_DEBUG( "Got a Z candidate" );
	    nZ++;
	    m_TagAndProbe_ID_Selected_ZMass->Fill(invMass);
	    myZMuons.push_back(std::make_pair(track1,track2));
	  }
	}
      }
    }
  }//close for
  
  m_TagAndProbe_ID_NCandidates->Fill(nZ);
  if (myZMuons.size()==0) {
    ATH_MSG_DEBUG( "No Z candidates" );
    return sc;
  }  
  ATH_MSG_INFO( "Found " << nZ << " Z candidates" );
  nZReco +=nZ;
 
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
  for (unsigned int i=0; i<myZMuons.size(); i++) {
    //    const Rec::TrackParticle *tag = myZMuons[i].first;
    const Rec::TrackParticle *probe = myZMuons[i].second;
    
    //all probe muons
    m_h_eta_tot->Fill(probe->eta(), 1.);
    m_h_pt_tot->Fill(probe->pt(), 1.);
    m_h_phi_tot->Fill(probe->phi(), 1.);
    
    if (isAssociatedWithMSTrack(probe)) {
      ATH_MSG_DEBUG( "Probe ID muon associated to a MS track");
      //probe muons reconstructed in MS
      m_h_eta_recon->Fill(probe->eta() , 1.);
      m_h_pt_recon->Fill(probe->pt() , 1.);
      m_h_phi_recon->Fill(probe->phi(), 1.);   
      m_TagAndProbe_Cut_Flow -> Fill(6);
      /*
	if (m_doTruth && isTrueZ[i]) {
	m_h_etatruth->Fill(probe->eta(), 1.);
	m_h_pttruth->Fill(probe->pt(), 1.);
	m_h_phitruth->Fill(probe->phi(), 1.);
	}
      */
    }
  }

  return StatusCode::SUCCESS;
} //=============end of fillHistograms()======================================================
//////////////////////////////////////////////////////////////////////////////////////////////
StatusCode MuonInsituPerformance::procHistograms()
{
  ATH_MSG_DEBUG( "in procHistograms()" );
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
    if(endOfEventsBlock){}
    if(endOfLumiBlock){}

    if(endOfRun){
      for (unsigned int j=1; j<=m_binsEta.size(); j++)
	if (m_h_eta_tot->GetBinContent(j)>0) {
	  m_TagAndProbe_Efficiency_Eta->Fill(m_binsEta[j-1], m_h_eta_recon->GetBinContent(j) / m_h_eta_tot->GetBinContent(j) );
	}
      m_TagAndProbe_Efficiency_Eta->SetMarkerStyle(2);
      m_TagAndProbe_Efficiency_Eta->SetOption("P");
      m_TagAndProbe_Efficiency_Eta->GetYaxis()->SetRangeUser(0.,1.1);

      ATH_MSG_DEBUG( "in procHistograms() 1" );
      
      for (unsigned int j=1; j<=m_binsPhi.size(); j++)
	if (m_h_phi_tot->GetBinContent(j)>0) {
	  m_TagAndProbe_Efficiency_Phi->Fill(m_binsPhi[j-1], m_h_phi_recon->GetBinContent(j) / m_h_phi_tot->GetBinContent(j));
	}
      m_TagAndProbe_Efficiency_Phi->SetMarkerStyle(2);
      m_TagAndProbe_Efficiency_Phi->SetOption("P");
      m_TagAndProbe_Efficiency_Phi->GetYaxis()->SetRangeUser(0.,1.1);

      ATH_MSG_DEBUG( "in procHistograms() 2" );

      for (unsigned int j=1; j<=m_binsPt.size(); j++) 
	if (m_h_pt_tot->GetBinContent(j)>0) {
	  m_TagAndProbe_Efficiency_Pt->Fill(m_binsPt[j-1], m_h_pt_recon->GetBinContent(j) / m_h_pt_tot->GetBinContent(j));
	}
      m_TagAndProbe_Efficiency_Pt->SetMarkerStyle(2);
      m_TagAndProbe_Efficiency_Pt->SetOption("P");
      m_TagAndProbe_Efficiency_Pt->GetYaxis()->SetRangeUser(0.,1.1);

      ATH_MSG_DEBUG( "in procHistograms() 3" );

      if (m_doTruth) {
	for (unsigned int j=1; j<=m_binsEta.size(); j++)
	  if (m_h_eta_tot->GetBinContent(j)>0) {
	    m_TagAndProbe_Efficiency_Eta_Truth->Fill(m_binsEta[j-1], m_h_eta_truth->GetBinContent(j) / m_h_eta_tot->GetBinContent(j)); 
	    m_TagAndProbe_Efficiency_Eta_Truth->SetMarkerStyle(3);
	    m_TagAndProbe_Efficiency_Eta_Truth->SetMarkerColor(kRed);
	    m_TagAndProbe_Efficiency_Eta_Truth->SetOption("P");
	    m_TagAndProbe_Efficiency_Eta_Truth->GetYaxis()->SetRangeUser(0.,1.1);
	  }   
	ATH_MSG_DEBUG( "in procHistograms() 4" );

	for (unsigned int j=1; j<=m_binsPhi.size(); j++)
	  if (m_h_phi_tot->GetBinContent(j)>0) {
	    m_TagAndProbe_Efficiency_Phi_Truth->Fill(m_binsPhi[j-1], m_h_phi_truth->GetBinContent(j) / m_h_phi_tot->GetBinContent(j));
	  }
	m_TagAndProbe_Efficiency_Phi_Truth->SetMarkerStyle(3);
	m_TagAndProbe_Efficiency_Phi_Truth->SetMarkerColor(kRed);
	m_TagAndProbe_Efficiency_Phi_Truth->SetOption("P");
	m_TagAndProbe_Efficiency_Phi_Truth->GetYaxis()->SetRangeUser(0.,1.1);
	ATH_MSG_DEBUG( "in procHistograms() 5" );

	for (unsigned int j=1; j<=m_binsPt.size(); j++) 
	  if (m_h_pt_tot->GetBinContent(j)>0) {
	    m_TagAndProbe_Efficiency_Pt_Truth->Fill(m_binsPt[j-1], m_h_pt_truth->GetBinContent(j) / m_h_pt_tot->GetBinContent(j));
	  }
	m_TagAndProbe_Efficiency_Pt_Truth->SetMarkerStyle(3);
	m_TagAndProbe_Efficiency_Pt_Truth->SetMarkerColor(kRed);
	m_TagAndProbe_Efficiency_Pt_Truth->SetOption("P");
	m_TagAndProbe_Efficiency_Pt_Truth->GetYaxis()->SetRangeUser(0.,1.1);
    
	ATH_MSG_DEBUG( "in procHistograms() 7" );

	m_h_pt_truth->Delete();
	m_h_eta_truth->Delete();
	m_h_phi_truth->Delete();
	// m_h_pt_truth=0;
	// m_h_eta_truth=0;
	// m_h_phi_truth=0;  
      }
      //      ATH_MSG_DEBUG( "in procHistograms() 8" );
      //delete temp hists
      // m_h_pt_tot->Delete();
      //      ATH_MSG_DEBUG( "in procHistograms() 9" );
      // m_h_eta_tot->Delete();
      //      ATH_MSG_DEBUG( "in procHistograms() 10" );
      // m_h_phi_tot->Delete();
      //      ATH_MSG_DEBUG( "in procHistograms() 11" );
      // m_h_pt_recon->Delete();
      //      ATH_MSG_DEBUG( "in procHistograms() 12" );
      // m_h_eta_recon->Delete();
      //      ATH_MSG_DEBUG( "in procHistograms() 13" );
      // m_h_phi_recon->Delete();
    
      // m_h_pt_tot=0;
      // m_h_eta_tot=0;
      // m_h_phi_tot=0;  
      // m_h_pt_recon=0;
      // m_h_eta_recon=0;
      // m_h_phi_recon=0;
      /*
	m_h_pttrig->Delete();
	m_h_etatrig->Delete();
	m_h_phitrig->Delete();
	m_h_pttrig=0;
	m_h_etatrig=0;
	m_h_phitrig=0;
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
      //    ATH_MSG_DEBUG( "in procHistograms() 14" );
    }  // isEndOfRun
  } // m_environment
  return StatusCode::SUCCESS;
} //=============end of procHistograms()======================================================
//////////////////////////////////////////////////////////////////////////////////////////////
StatusCode MuonInsituPerformance::finalize()
{
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------

bool MuonInsituPerformance::isAssociatedWithMSTrack(const Rec::TrackParticle *trk) 
{  
  /* 
     - loop on combined muon container; 
     - get the pointer from the combined muon to its inner detector track; 
     - check if it matches the probe ID-track; 
     - if yes, then the probe track is associated with a muon spectrometer track or low-pt muon track 
  */

  ATH_MSG_DEBUG( "in isAssociatedWithMStrack()" );

  const Analysis::MuonContainer* importedMuonContainer;
  if(m_storeGate->retrieve(importedMuonContainer, m_MuonContainer ).isFailure()){
    ATH_MSG_ERROR( "Couldn't retrieve MuonContainer from StoreGate" );
    return StatusCode::FAILURE;
  }
  
  Analysis::MuonContainer::const_iterator muItr;
  for ( muItr=importedMuonContainer->begin(); muItr!=importedMuonContainer->end(); muItr++) {
    if ((*muItr)->hasInDetTrackParticle()) {
      //      if ( (*muItr)->combinedMuonTrackParticle() == trk ) {
      if ( (*muItr)->inDetTrackParticle()==trk) {
	return true;
      }
    }
  }
  return false;
}
//======================================================================
bool MuonInsituPerformance::isElectron(const Rec::TrackParticle *track)
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

std::pair<double,double> MuonInsituPerformance::getTrackPropertiesInCone(const Rec::TrackParticle *track)
{
  //return number of tracks and sum of pt inside a delta-R cone around the track
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG( "in getTrackPropertiesInCone()");
  const Rec::TrackParticleContainer* importedInDetTrackContainer=0;
  sc = m_storeGate->retrieve(importedInDetTrackContainer, m_InnerTrackContainer);
  if( sc.isFailure() || !importedInDetTrackContainer )  {
    ATH_MSG_ERROR( "Can't retrieve TrackParticle container from StoreGate." );
    return std::make_pair(-1,-1);
  }

  Rec::TrackParticleContainer::const_iterator trkItr   = importedInDetTrackContainer->begin();
  Rec::TrackParticleContainer::const_iterator trkItrE  = importedInDetTrackContainer->end();

  double nTracks=0;
  double sumPt=0;
  for(; trkItr != trkItrE; trkItr++) {
    double dR = sqrt( pow(track->eta() - (*trkItr)->eta(),2) + pow(track->phi() - (*trkItr)->phi(),2) );
    double relPt = ( (*trkItr)->pt() - track->pt() )/track->pt();
    if ( dR<1e-6 && relPt<1e-6 ) { ATH_MSG_DEBUG( "Not counting the ID track itself inside the cone" ); continue; }
    if ( dR>m_isoConeMin && dR<m_isoConeMax && relPt>0.1) {
      nTracks++;
      sumPt+=(*trkItr)->pt();
    }
  }
  return std::make_pair(nTracks,sumPt);
}

double MuonInsituPerformance::getJetEnergyInCone(const Rec::TrackParticle *track) const 
{
  ATH_MSG_DEBUG( "in getJetEnergyInCone()");

  if(!evtStore()->contains<JetCollection>(m_JetContainer) ) return 0;

  const JetCollection* mcpartJet=0; 
  if( evtStore()->retrieve(mcpartJet,m_JetContainer).isFailure()) {
    ATH_MSG_WARNING("Cannot retrieve Jet container with key "+m_JetContainer);
    return 0;
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
  
double MuonInsituPerformance::getETIsolation(const Analysis::Muon *muon)
{
  ATH_MSG_DEBUG( "in getETIsolation()");
  return muon->parameter( MuonParameters::etcone40 );
}
