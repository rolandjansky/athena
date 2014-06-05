/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////
// Authors:  I.Nomidis (AUTh) - N. Benekos(Illinois)
// MuonDQA
// Mar. 2009
//
// DESCRIPTION:
// Subject: 	This class does a full Zmumu-Analysis based on AODs.
//
// Modified:    Venkat Kaushik (Venkat.Kaushik@@cern.ch) on Mon Mar 31 15:37:27 CEST 2008 
// Notes:       Converted the ZmumuValidator into a monitoring tool. This class derives
// 		from ManagedMonitorToolBase class and is used for booking/filling monitoring
//              histograms
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include "MuonPhysicsMonitoring/ZmumuPhysMonTool.h"

/////////////////////////////////////////////////////////////////////////////

ZmumuPhysMonTool::ZmumuPhysMonTool(const std::string& type, const std::string& name, const IInterface* parent) 
  : ManagedMonitorToolBase(type, name, parent) 
{
  declareProperty("doMCTruth",                  m_doTruth = false);

  declareProperty( "InnerTrackContainer" ,      m_InnerTrackContainer );
  declareProperty( "MuonSpectrometerContainer", m_MuonSpectrometerTrackContainer );
  declareProperty( "MuonContainer",	        m_muonCollectionName);
  declareProperty( "ElectronContainer",         m_ElectronContainer);
  declareProperty( "JetContainer",              m_JetContainer);
  declareProperty( "muonDeltaPhiCut",	        m_cut_deltaPhi = 0);
  declareProperty( "diMuonMassWindow",	        m_cut_diMuonMass = 0);
  declareProperty( "muonHigherPt",              m_cut_muonHigherPt );
  declareProperty( "muonLowerPt" ,              m_cut_muonLowerPt); //preselection pt-cut on muons
  declareProperty( "muonEtaCut",                m_cut_muonEta = 0); //preselection eta-cut on muons 
  declareProperty( "isolationJet",              m_cut_isolationJet = 0. );
  declareProperty( "isolationNtracks",          m_cut_isolationNtracks = 0. );
  declareProperty( "isolationPtSum",            m_cut_isolationPtSum = 0.);
  declareProperty( "isolationET",               m_cut_isolationET = 0.);
  //isolation
  declareProperty( "isoConeMax",                m_isoConeMax = 0);
  declareProperty( "isoConeMin",                m_isoConeMin = 0);

  declareProperty( "binsEta",                   m_binsEta );
  declareProperty( "binsPhi",                   m_binsPhi );
  declareProperty( "binsPt",                    m_binsPt );

  declareProperty( "useTrigger",                m_doTrigger );
  declareProperty( "TriggerSelection",          m_trigSelection );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/*******************************/

//======================================================================================//
/**  Initialize */
//======================================================================================//
StatusCode ZmumuPhysMonTool::initialize() 
{ 
  StatusCode sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  ATH_MSG_INFO( "in initialize()" );

  /** Initialize tools and services */
  //Storegate
  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to retrieve pointer to StoreGateSvc" );
    return sc;
  }

  //Histogram service
  sc = service("THistSvc", m_thistSvc);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to retrieve pointer to THistSvc" );
    return sc;
  }

  /// get a handle on the analysis tools
  IToolSvc* toolSvc;
  sc = service("ToolSvc",toolSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR( " Can't get ToolSvc" );
    return StatusCode::FAILURE;
  }

  /*
    IAlgTool *tmp_analysisTools;
    sc = toolSvc->retrieveTool("AnalysisTools",tmp_analysisTools);
    if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR( "Can't get handle on analysis tools" );
    return StatusCode::FAILURE;
    }
    m_analysisTools=dynamic_cast<IAnalysisTools *>(tmp_analysisTools);
  */

  if(m_doTruth) {
    /*
      sc = toolSvc->retrieveTool( "TruthParticleCnvTool/CnvTool", tmp_analysisTools, this );
      if (StatusCode::SUCCESS != sc) {
      ATH_MSG_ERROR( "Creation of algTool ITruthParticleCnvTool FAILED !" );
      return StatusCode::FAILURE;
      } else {
      ATH_MSG_INFO( "AlgTool ITruthParticleCnvTool successfully created" );
      m_cnvTool = dynamic_cast<ITruthParticleCnvTool*>(tmp_analysisTools);
      }
    */
  }
  if ( m_doTrigger ) {
    //m_all   = m_trigDecTool->getChainGroup(".*");
    //m_allL1 = m_trigDecTool->getChainGroup("L1_MU.*");
    //m_allL2 = m_trigDecTool->getChainGroup("L2_.*");
    //m_allEF = m_trigDecTool->getChainGroup("EF_.*");
  }

  nEvents = 0;
    
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

StatusCode ZmumuPhysMonTool::bookHistogramsRecurrent()
{
  ATH_MSG_DEBUG( "in bookHistograms()" );
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD) {
    if (newEventsBlock){}
    if (newLumiBlock){}
    if (newRun) {
    
      std::string m_generic_path_muonphysicsmonitoring = "Muon/MuonPhysicsMonitoring/Zmumu/";
      std::string path;

      for (unsigned int j=0; j<m_muonCollectionName.size(); j++) {
	StatusCode sc = StatusCode::SUCCESS;

	if (m_doTrigger && m_trigSelection!="") {
	  path = m_generic_path_muonphysicsmonitoring+"/Overview/TriggerAware/"+m_muonCollectionName[j]+m_trigSelection+"/";
	}
	else path = m_generic_path_muonphysicsmonitoring+"/Overview/NoTrigger/"+m_muonCollectionName[j]+"/";
	
	///
	MonGroup muonphysmon_shift( this, path, run, ATTRIB_UNMANAGED );
	MonGroup muonphysmon_expert( this, path, run, ATTRIB_UNMANAGED );
	             
	//define hists
	/*
	  if (m_doTrigger) {       
	  m_Trig_Cut_Flow = new TH1F( "m_Trig_Cut_Flow","Trigger Cut flow (1:AllEvents, 2:passedL1 3:passedL2 4:passedEF )" , 5,0.5,5.5);
	  sc=muonphysmon_shift.regHist(m_Trig_Cut_Flow);
	  }
	*/
	//Truth
	if(m_doTruth) {
	  m_True_NumberOfMuons.push_back( new TH1F( "m_True_NumberOfMuons" , "Number of True Muons >6GeV" ,100,0,100));
	  m_True_ZMass     .push_back( new TH1F( "m_True_ZMass" , "True Z Mass [MeV]" ,100,0,10000));
	  m_True_DiMuonMass   .push_back( new TH1F( "m_True_DiMuonMass" , "True DiMuon Mass [MeV]" ,100,0,6000));
	  m_True_ZPt       .push_back( new TH1F( "m_True_ZPt" , "True Z P_{T} [MeV]",100,2000,4000));
	  m_True_MuonPt       .push_back( new TH1F( "m_True_Muon_Pt" , "True Z Muon P_{T} [MeV]" , 100,0,10000));
	  sc=muonphysmon_expert.regHist( m_True_NumberOfMuons.back());
	  sc=muonphysmon_expert.regHist( m_True_ZMass.back());
	  sc=muonphysmon_expert.regHist( m_True_DiMuonMass.back());
	  sc=muonphysmon_expert.regHist( m_True_ZPt.back());
	  sc=muonphysmon_expert.regHist( m_True_MuonPt.back());
	}
	
	m_Reco_DiMuonMass.push_back( new TH1F( "m_Reco_DiMuonMass" , "Reconstructed DiMuon Mass [MeV]" , 100,0,120000)); //before cuts, oppos.charged muon pairs
	m_Reco_ZMass.push_back( new TH1F( "m_Reco_ZMass" , "Reconstructed Mass of Z candidates [MeV]" , 100,91000-m_cut_diMuonMass,91000+m_cut_diMuonMass)); //Z candidates, after cuts
	m_nCandidates_perEvent.push_back( new TH1F( "m_nCandidates_perEvent", "Number of candidates per event", 10, 0, 10));
	m_Reco_NumberOfTracks .push_back( new TH1F( "m_Reco_NumberOfTracks" , "Number of Reconstructed Tracks" , 10,0,10));
	m_Reco_DiMuonPt       .push_back( new TH1F( "m_Reco_DiMuonPt" , "Reconstructed DiMuon Pt [MeV]" , 100,0,150000));
	m_Reco_PtMin          .push_back( new TH1F( "m_Reco_PtMin" , "Reconstructed Minimal Muon Pt [MeV]" , 100,0,80000));
	m_Reco_PtMax          .push_back( new TH1F( "m_Reco_PtMax" , "Reconstructed_Maximal Muon Pt [MeV]" , 100,0,100000));
	m_Reco_DeltaPhi         .push_back( new TH1F( "m_Reco_DeltaPhi" , "Reconstructed #Delta #phi",40,0.,4.));
	m_Reco_DeltaPhi_vs_Pt   .push_back( new TH2F( "m_Reco_DeltaPhi_vs_pt","Reconstructed #Delta #phi versus p_{T} [MeV]", 100,m_cut_muonLowerPt,100000.,30,0.,4.));
	m_Reco_Charge         .push_back( new TH1F( "m_Reco_Charge" , "Reconstructed Charge_{1} x Charge_{2}" , 3,-1.5,1.5));
	m_Reco_MuonPt         .push_back( new TH1F( "m_Reco_MuonPt", "Reconstructed Muon Pt [MeV]", 150,0,150000));
	m_Reco_MuonEta         .push_back( new TH1F( "m_Reco_MuonEta", "Reconstructed Muon Eta", 54, -2.7, 2.7));
	m_Cut_Efficiency      .push_back( new TH1F( "m_Cut_Efficiency" , "Number of Events (1:AllEvents, 2:Z candidates, 3:Z matched with truth, 4:Fake Z, 5:Two Z)" , 6,0.5,6.5));
	m_Cut_Flow            .push_back( new TH1F( "m_Cut_Flow","Cut flow (1:#mu^{+}#mu^{-}, 2:p_{T}, 3:Mass, 4:JetIso, 5:NtrackIso, 6:p_{T}Iso, 7:E_{T}Iso, 8:!electron, 9:#Delta #phi )" , 10,0.5,10.5));
	
	sc=muonphysmon_shift.regHist( m_Reco_DiMuonMass.back());
	sc=muonphysmon_shift.regHist( m_Reco_ZMass.back());
	sc=muonphysmon_expert.regHist( m_nCandidates_perEvent.back());
	sc=muonphysmon_expert.regHist( m_Reco_NumberOfTracks.back());
	sc=muonphysmon_expert.regHist( m_Reco_DiMuonPt.back());
	sc=muonphysmon_expert.regHist( m_Reco_PtMin.back());
	sc=muonphysmon_expert.regHist( m_Reco_PtMax.back());
	sc=muonphysmon_expert.regHist( m_Reco_DeltaPhi.back());
	sc=muonphysmon_expert.regHist( m_Reco_DeltaPhi_vs_Pt.back());
	sc=muonphysmon_expert.regHist( m_Reco_Charge.back());
	sc=muonphysmon_expert.regHist( m_Reco_MuonPt.back());
	sc=muonphysmon_expert.regHist( m_Reco_MuonEta.back());
	sc=muonphysmon_expert.regHist( m_Cut_Efficiency.back());
	sc=muonphysmon_shift.regHist( m_Cut_Flow.back());
      
	m_Reco_JetEnergy_InHalo.push_back(new TH1F( "m_Reco_JetEnergy_InHalo" , "Reconstructed Jet Energy in Halo around Muon [GeV]" , 100,0,50000));
	m_Reco_NumberOfTracks_InHalo.push_back(new TH1F( "m_Reco_NumberOfTracks_InHalo" , "Number Of Reconstructed Tracks In Halo" , 20,0,20));
	m_Reco_TrackPt_InHalo.push_back(new TH1F( "m_Reco_TrackPt_InHalo" , "Sum of Track p_{T}'s in Halo around Muon [GeV]" , 100,0,50000));
	m_Reco_ET_InHalo.push_back(new TH1F( "m_Reco_ET_InHalo" , "Reconstructed ET in Halo around Muon [GeV]" , 100,0,50000));
	sc=muonphysmon_shift.regHist(m_Reco_JetEnergy_InHalo.back());
	sc=muonphysmon_shift.regHist(m_Reco_NumberOfTracks_InHalo.back());
	sc=muonphysmon_shift.regHist(m_Reco_TrackPt_InHalo.back());
	sc=muonphysmon_shift.regHist(m_Reco_ET_InHalo.back());

	m_Reco_Mass_vs_Pt.push_back( new TProfile("m_Reco_Mass_vs_Pt","Reconstructed Z mass profile vs p_{T} [MeV]",m_binsPt.size()-1 , &m_binsPt[0] )) ;
	m_Reco_Mass_vs_Eta.push_back( new TProfile("m_Reco_Mass_vs_Eta","Reconstructed Z mass profile vs #eta", m_binsEta.size()-1 , &m_binsEta[0] )) ;
	m_Reco_Mass_vs_Phi.push_back( new TProfile("m_Reco_Mass_vs_Phi","Reconstructed Z mass profile vs #phi", m_binsPhi.size()-1 , &m_binsPhi[0] )) ;
	m_Reco_MassShift_vs_Pt.push_back( new TH1F("m_Reco_MassShift_vs_Pt","Reconstructed Z mass shift vs p_{T} [MeV]",m_binsPt.size()-1 , &m_binsPt[0] )) ;
	m_Reco_MassShift_vs_Eta.push_back( new TH1F("m_Reco_MassShift_vs_Eta","Reconstructed Z mass shift vs #eta", m_binsEta.size()-1 , &m_binsEta[0] )) ;
	m_Reco_MassShift_vs_Phi.push_back( new TH1F("m_Reco_MassShift_vs_Phi","Reconstructed Z mass shift vs #phi", m_binsPhi.size()-1 , &m_binsPhi[0] )) ;
      
	sc=muonphysmon_shift.regHist(m_Reco_Mass_vs_Pt.back());
	sc=muonphysmon_shift.regHist(m_Reco_Mass_vs_Eta.back());
	sc=muonphysmon_shift.regHist( m_Reco_Mass_vs_Phi.back());
	sc=muonphysmon_shift.regHist(m_Reco_MassShift_vs_Pt.back());
	sc=muonphysmon_shift.regHist(m_Reco_MassShift_vs_Eta.back());
	sc=muonphysmon_shift.regHist( m_Reco_MassShift_vs_Phi.back());
      
	//Trigger efficiencies
	/*
	  if (m_trigSelection.size()>1 && m_doTrigger==true && iTrig>0) {
	  //no histograms for NoTriggerSelection
	  MonGroup muonphysmon_trigefficiencies( this, m_generic_path_muonphysicsmonitoring+"Efficiencies/Trigger/"+m_trigSelection[iTrig]+"/", shift, run );

	  m_Z_TrigEfficiency_Eta.push_back( new TH1F( "m_Z_TrigEfficiency_Eta" ,"Z_TrigEfficiency_Eta" , m_binsEta.size()-1 , &m_binsEta[0] ));
	  sc=muonphysmon_trigefficiencies.regHist(m_Z_TrigEfficiency_Eta.back());
	
	  m_Z_TrigEfficiency_Phi.push_back( new TH1F( "m_Z_TrigEfficiency_Phi" ,"Z_TrigEfficiency_Phi" , m_binsPhi.size()-1 , &m_binsPhi[0] ));
	  sc=muonphysmon_trigefficiencies.regHist(m_Z_TrigEfficiency_Phi.back());
	
	  m_Z_TrigEfficiency_Pt.push_back( new TH1F( "m_Z_TrigEfficiency_Pt" ,"Z_TrigEfficiency_Pt" , m_binsPt.size()-1 , &m_binsPt[0] ));
	  sc=muonphysmon_trigefficiencies.regHist(m_Z_TrigEfficiency_Pt.back());
	
	  std::string hnamePt="m_h_pttrig_"+m_trigSelection[iTrig];
	  m_h_pttrig.push_back( new TH1F(hnamePt.c_str(), "",m_binsPt.size()-1, &m_binsPt[0] ));
	  std::string hnameEta="m_h_etatrig_"+m_trigSelection[iTrig];
	  m_h_etatrig.push_back( new TH1F(hnameEta.c_str(), "",m_binsEta.size()-1, &m_binsEta[0] ));
	  std::string hnamePhi="m_h_phitrig_"+m_trigSelection[iTrig];
	  m_h_phitrig.push_back( new TH1F(hnamePhi.c_str(), "",m_binsPhi.size()-1, &m_binsPhi[0] ));
	  }
	*/
	if(sc.isFailure()) { ATH_MSG_FATAL( "Failed to register histogram(s)" ); return sc; }
	
      } //loop over muon collections
    
      /*
	if (m_doTrigger) {
	m_numZ_per_TriggerItem = new TH1F("m_numZ_per_TriggerItem","Number of Z per trigger item",1,0,1);
	sc=muonphysmon.regHist( m_numZ_per_TriggerItem );
	m_numDimuons_per_TriggerItem = new TH1F("m_numDimuons_per_TriggerItem","Number of di-muons per trigger item",1,0,1);
	sc=muonphysmon.regHist( m_numDimuons_per_TriggerItem );
	}
      */
      /*
	m_h_pttotal = new TH1F("m_h_pttotal", "",m_binsPt.size()-1, &m_binsPt[0] );
	m_h_etatotal = new TH1F("m_h_etatotal", "",m_binsEta.size()-1, &m_binsEta[0] );
	m_h_phitotal = new TH1F("m_h_phitotal", "",m_binsPhi.size()-1, &m_binsPhi[0] );
      */
    } //NewRun
  }//environment

  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode ZmumuPhysMonTool::fillHistograms()
{
  ATH_MSG_DEBUG( "in fillHistograms()" );

  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD) {

    //=============================================
    if (m_doTrigger && m_trigSelection!="") {
      if (m_trigDecTool->isPassed(m_trigSelection)==false) {
	ATH_MSG_INFO( "Event didn't pass trigger "<< m_trigSelection <<". Skipping event..." );
	return StatusCode::SUCCESS;
      }
    }
    //=============================================   

    for (unsigned int i=0; i<m_muonCollectionName.size(); i++) {
      ATH_MSG_INFO( "Retrieving Muon Collection: " << m_muonCollectionName[i] ); 
      const Analysis::MuonContainer* importedMuonContainer;
      StatusCode sc = m_storeGate->retrieve(importedMuonContainer,  m_muonCollectionName[i] );
      if (sc.isFailure() || !importedMuonContainer) {
	ATH_MSG_WARNING( "No MuonCollection " << m_muonCollectionName[i] << " found in StoreGate" );
	return StatusCode::SUCCESS;
      }

      ATH_MSG_DEBUG( "Found "<< importedMuonContainer->size() << " muons in container" );

      m_Cut_Efficiency[i]->Fill(1); //number of all events
    
      std::vector<const Analysis::Muon*> myMuons;
      Analysis::MuonContainer::const_iterator muItr;
    
      // Loop on Staco/Muid collections, keep only combined and low-pt muons, apply preselection cut on pt and eta
      for ( muItr=importedMuonContainer->begin(); muItr!=importedMuonContainer->end(); muItr++) {
	if( (*muItr)->isCombinedMuon() ) { // || (*muItr)->isStandAloneMuon()) {
	  m_Reco_MuonPt[i]->Fill( (*muItr)->pt() );
	  m_Reco_MuonEta[i]->Fill( (*muItr)->eta() );
	
	  if ( (*muItr)->pt()>m_cut_muonLowerPt ) {
	    if ( (*muItr)->eta()<m_cut_muonEta ) {
	      myMuons.push_back((*muItr));
	    }
	  }
	}
      }
      ATH_MSG_DEBUG( "Got "<< myMuons.size() << " muons after preselection" );
      //m_Reco_NumberOfTracks[i]->Fill(myMuons.size());
    
      if (myMuons.size()<2) {
	ATH_MSG_DEBUG( "Can't make pair. Skipping event..." );
	return StatusCode::SUCCESS;
      }
    
      //make pairs of oppositely charged muons
      double charge1=0.;
      double charge2=0.;
      std::vector< std::pair<const Analysis::Muon*,const Analysis::Muon*> > myMuonPairs;
      for (unsigned int imu=0; imu<myMuons.size()-1; imu++) {
	charge1 = myMuons[imu]->charge();
	ATH_MSG_DEBUG( "Got muon1 with q=" <<charge1 );
	for (unsigned int jmu=imu+1; jmu<myMuons.size(); jmu++) {
	  charge2=myMuons[jmu]->charge();
	  ATH_MSG_DEBUG( "Got muon2 with q=" <<charge2 );
	
	  m_Reco_Charge[i]->Fill(charge1*charge2);
	  if (charge1*charge2<0) {
	    ATH_MSG_DEBUG( "Got pair of oppositely charged muons" );
	  
	    //sort muons according to pt, the one with the higher pt goes first
	    if (myMuons[imu]->pt() > myMuons[jmu]->pt()) myMuonPairs.push_back(std::make_pair(myMuons[imu],myMuons[jmu]));
	    else myMuonPairs.push_back(std::make_pair(myMuons[jmu],myMuons[imu]));
	  }
	}
      }
      ATH_MSG_DEBUG( "Found " << myMuonPairs.size() << " muon pairs" );
      if (myMuonPairs.size()==0) {
	ATH_MSG_DEBUG( "No pairs, skipping event..." );
	return StatusCode::SUCCESS;
      }
    
      //loop over muon pairs, apply cuts and find Z candidates
      int nZ=0;
     // double eta1=0.;
     // double eta2=0.;
    
      std::vector< std::pair<const Analysis::Muon*,const Analysis::Muon*> > myZMuons;
      for (unsigned int iPair=0; iPair<myMuonPairs.size(); iPair++) {
	ATH_MSG_DEBUG( "Getting pair #"<< (iPair+1) );
      
	const Analysis::Muon* muon1 = myMuonPairs[iPair].first;
	const Analysis::Muon* muon2 = myMuonPairs[iPair].second;
      
	if (!muon1 || !muon2) {
	  ATH_MSG_DEBUG( "Null muon, skipping pair" );
	  continue;
	}
      
	if (!muon1->hasInDetTrackParticle() || !muon2->hasInDetTrackParticle()) {
	  ATH_MSG_DEBUG( "Null ID muon track, skipping pair" );
	  continue;
	}
      
	const Rec::TrackParticle *muonIdTrack1 = muon1->inDetTrackParticle();
	const Rec::TrackParticle *muonIdTrack2 = muon2->inDetTrackParticle();
     
	double pt1 = muon1->pt();
	double pt2 = muon2->pt();
	double pt = ( muon1->hlv() + muon2->hlv() ).perp();
	//eta1 = muon1->eta();
	//eta2 = muon2->eta();
	double phi1 = muon1->phi();
	double phi2 = muon2->phi();
	double deltaPhi=fabs(phi1-phi2);
	if (deltaPhi>3.14159) deltaPhi=6.28318-deltaPhi;
	double invMass = ( muon1->hlv() + muon2->hlv() ).m();
	double isolationJet1 = getJetEnergyInCone(muonIdTrack1);
	double isolationJet2 = getJetEnergyInCone(muonIdTrack2);
    
	std::pair<double,double> propertiesInCone1 = getTrackPropertiesInCone(muonIdTrack1);
	std::pair<double,double> propertiesInCone2 = getTrackPropertiesInCone(muonIdTrack2);
	double isolationNtracks1 = propertiesInCone1.first;
	double isolationNtracks2 = propertiesInCone2.first;
	double isolationPtSum1 = propertiesInCone1.second;
	double isolationPtSum2 = propertiesInCone2.second;    
	double isolationET1 = getETIsolation(muon1);
	double isolationET2 = getETIsolation(muon2);

	//Keep quantities before cuts
	m_Reco_DiMuonMass[i]->Fill(invMass);
	m_Reco_DeltaPhi[i]->Fill(deltaPhi);
	m_Reco_PtMax[i]->Fill(pt1);
	m_Reco_PtMin[i]->Fill(pt2);
	m_Reco_DiMuonPt[i]->Fill(pt);
	m_Reco_DeltaPhi_vs_Pt[i]->Fill(pt1,deltaPhi);
	m_Reco_DeltaPhi_vs_Pt[i]->Fill(pt2,deltaPhi);
      
	//isolation quantities
	m_Reco_JetEnergy_InHalo[i]->Fill(isolationJet1);
	m_Reco_JetEnergy_InHalo[i]->Fill(isolationJet2);
	m_Reco_NumberOfTracks_InHalo[i]->Fill(isolationNtracks1);
	m_Reco_NumberOfTracks_InHalo[i]->Fill(isolationNtracks2);
	m_Reco_TrackPt_InHalo[i]->Fill(isolationPtSum1);
	m_Reco_TrackPt_InHalo[i]->Fill(isolationPtSum2);
	m_Reco_ET_InHalo[i]->Fill(isolationET1);
	m_Reco_ET_InHalo[i]->Fill(isolationET2);

	//fill histograms for muon pairs, before cuts
	m_Cut_Flow[i]->Fill(1);
      
	//apply cuts
	ATH_MSG_DEBUG( "Applying cuts..." );
	if (pt1>m_cut_muonHigherPt && pt2>m_cut_muonLowerPt) {
	  m_Cut_Flow[i]->Fill(2);
	  if ( fabs(invMass-91000) < m_cut_diMuonMass ) {
	    m_Cut_Flow[i]->Fill(3);
	    if (isolationJet1<m_cut_isolationJet && isolationJet2<m_cut_isolationJet) {
	      m_Cut_Flow[i]->Fill(4);
	      if (isolationNtracks1<m_cut_isolationNtracks && isolationNtracks2<m_cut_isolationNtracks) {
		m_Cut_Flow[i]->Fill(5);
		if (isolationPtSum1<m_cut_isolationPtSum && isolationPtSum2<m_cut_isolationPtSum) {
		  m_Cut_Flow[i]->Fill(6);
		  if (isolationET1<m_cut_isolationET && isolationET2<m_cut_isolationET) {
		    m_Cut_Flow[i]->Fill(7);
		    if ( !isElectron(muonIdTrack1) && !isElectron(muonIdTrack2) ) {
		      m_Cut_Flow[i]->Fill(8);
		      if ( deltaPhi > m_cut_deltaPhi ) {
			m_Cut_Flow[i]->Fill(9);
			ATH_MSG_DEBUG( "Got a Z candidate" );
			nZ++;
			m_Cut_Efficiency[i]->Fill(2);
			m_Reco_ZMass[i]->Fill(invMass); 
			myZMuons.push_back(std::make_pair(muon1,muon2));
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }//close for
    
      m_nCandidates_perEvent[i]->Fill(nZ);
      if (nZ>1) m_Cut_Efficiency[i]->Fill(4);
      if (myZMuons.size()==0) {
	ATH_MSG_DEBUG( "No Z candidates" );
	return StatusCode::SUCCESS;
      }  
      ATH_MSG_INFO( "Found " << nZ << " Z candidates" );
    
      for (unsigned int iZ=0; iZ<myZMuons.size(); iZ++) {
	//parameters of Z
	double invMass = ( myZMuons[iZ].first->hlv() + myZMuons[iZ].second->hlv() ).m();
	//	double invPt = ( myZMuons[iZ].first->hlv() + myZMuons[iZ].second->hlv() ).perp();
	//	double phi = ( myZMuons[iZ].first->hlv() + myZMuons[iZ].second->hlv() ).phi();
	//	double eta = ( myZMuons[iZ].first->hlv() + myZMuons[iZ].second->hlv() ).pseudoRapidity();
      
	//parameters of Z muons
	double Pt1 = myZMuons[iZ].first->pt();    double Pt2 = myZMuons[iZ].second->pt();
	double Eta1= myZMuons[iZ].first->eta();   double Eta2= myZMuons[iZ].second->eta();
	double Phi1= myZMuons[iZ].first->phi();   double Phi2= myZMuons[iZ].second->phi();
      
	m_Reco_Mass_vs_Pt[i]  -> Fill(Pt1, invMass, 0.5); 
	m_Reco_Mass_vs_Pt[i]  -> Fill(Pt2, invMass, 0.5);
	m_Reco_Mass_vs_Eta[i] -> Fill(Eta1, invMass, 0.5);
	m_Reco_Mass_vs_Eta[i] -> Fill(Eta2, invMass, 0.5);
	m_Reco_Mass_vs_Phi[i] -> Fill(Phi1, invMass, 0.5); 
	m_Reco_Mass_vs_Phi[i] -> Fill(Phi2, invMass, 0.5);
      
	//Trigger efficiency
	/*
	  if (m_doTrigger==true) {
	  ATH_MSG_DEBUG( "Counting events for trigger efficiency" );
	  if (i==0) { //noTriggerSelection
	  m_h_etatotal->Fill(eta, 1.);
	  m_h_pttotal->Fill(invPt, 1.);
	  m_h_phitotal->Fill(phi, 1.);
	  }
	  else {
	  //checking if this event was triggered
	  if ( m_trigDecTool->isPassed(m_trigSelection[i]) ) { 
	  //checking if it was one of the Z muons that fired the trigger
	  //....@@@?
	  m_h_etatrig[i-1]->Fill(eta, 1.);
	  m_h_pttrig[i-1]->Fill(invPt, 1.);
	  m_h_phitrig[i-1]->Fill(phi, 1.);	    
	  }
	  }
	  }
	*/
      }
    }
  }//environment
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode ZmumuPhysMonTool::procHistograms()
{
  ATH_MSG_DEBUG( "procHistograms()" );

  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
    if(endOfEventsBlock){}
    if(endOfLumiBlock){}
    if(endOfRun){
      /*
	for (unsigned int iTrig=0; iTrig<m_trigSelection.size(); iTrig++) {
	for (int ibin=1; ibin<=m_Reco_Mass_vs_Pt[iTrig]->GetNbinsX(); ibin++) {
	double dMassShift = m_Reco_Mass_vs_Pt[iTrig]->GetBinContent(ibin) - 91000;
	m_Reco_MassShift_vs_Pt[iTrig]   -> Fill(m_binsPt[ibin-1],  dMassShift);
	}
	m_Reco_MassShift_vs_Pt[iTrig]->SetMarkerStyle(2);
	m_Reco_MassShift_vs_Pt[iTrig]->SetOption("P");

	for (int ibin=1; ibin<=m_Reco_MassShift_vs_Eta[iTrig]->GetNbinsX(); ibin++) {
	double dMassShift = m_Reco_Mass_vs_Eta[iTrig]->GetBinContent(ibin) - 91000;
	m_Reco_MassShift_vs_Eta[iTrig]   -> Fill(m_binsEta[ibin-1],  dMassShift);
	}
	m_Reco_MassShift_vs_Eta[iTrig]->SetMarkerStyle(2);
	m_Reco_MassShift_vs_Eta[iTrig]->SetOption("P");
      
	for (int ibin=1; ibin<=m_Reco_MassShift_vs_Phi[iTrig]->GetNbinsX(); ibin++) {
	double dMassShift = m_Reco_Mass_vs_Phi[iTrig]->GetBinContent(ibin) - 91000;
	m_Reco_MassShift_vs_Phi[iTrig] -> Fill(m_binsPhi[ibin-1],  dMassShift);
	}
	m_Reco_MassShift_vs_Phi[iTrig]->SetMarkerStyle(2);
	m_Reco_MassShift_vs_Phi[iTrig]->SetOption("P");

	//Z trigger Efficiencies
	if (m_doTrigger==true && m_trigSelection.size()>1 && iTrig>0) {
	ATH_MSG_DEBUG("Dividing histos to get efficiencies");	
	for (int ibin=1; ibin<=m_Z_TrigEfficiency_Eta[iTrig-1]->GetNbinsX(); ibin++) {
	if (m_h_etatotal->GetBinContent(ibin)>0) {
	m_Z_TrigEfficiency_Eta[iTrig-1]->Fill(m_binsEta[ibin-1], m_h_etatrig[iTrig-1]->GetBinContent(ibin) / m_h_etatotal->GetBinContent(ibin));
	}
	}
	m_Z_TrigEfficiency_Eta[iTrig-1]->SetMarkerStyle(2);
	m_Z_TrigEfficiency_Eta[iTrig-1]->SetOption("P");
	m_Z_TrigEfficiency_Eta[iTrig-1]->GetYaxis()->SetRangeUser(0.,1.1);
	
	for (int ibin=1; ibin<=m_Z_TrigEfficiency_Phi[iTrig-1]->GetNbinsX(); ibin++) {
	if (m_h_phitotal->GetBinContent(ibin)>0) {
	m_Z_TrigEfficiency_Phi[iTrig-1]->Fill(m_binsPhi[ibin-1], m_h_phitrig[iTrig-1]->GetBinContent(ibin) / m_h_phitotal->GetBinContent(ibin));
	}
	}
	m_Z_TrigEfficiency_Phi[iTrig-1]->SetMarkerStyle(2);
	m_Z_TrigEfficiency_Phi[iTrig-1]->SetOption("P");
	m_Z_TrigEfficiency_Phi[iTrig-1]->GetYaxis()->SetRangeUser(0.,1.1);

	for (int ibin=1; ibin<=m_Z_TrigEfficiency_Pt[iTrig-1]->GetNbinsX(); ibin++) {
	if (m_h_pttotal->GetBinContent(ibin)>0) {
	m_Z_TrigEfficiency_Pt[iTrig-1]->Fill(m_binsPt[ibin-1], m_h_pttrig[iTrig-1]->GetBinContent(ibin) / m_h_pttotal->GetBinContent(ibin));
	}
	}
	m_Z_TrigEfficiency_Pt[iTrig-1]->SetMarkerStyle(2);
	m_Z_TrigEfficiency_Pt[iTrig-1]->SetOption("P");
	m_Z_TrigEfficiency_Pt[iTrig-1]->GetYaxis()->SetRangeUser(0.,1.1);
	
	m_h_pttrig[iTrig-1]->Delete();
	m_h_etatrig[iTrig-1]->Delete();
	m_h_phitrig[iTrig-1]->Delete();
	m_h_pttrig[iTrig-1]=0;
	m_h_etatrig[iTrig-1]=0;
	m_h_phitrig[iTrig-1]=0;
	}   
      */
      //print relative and absolut efficiencies
      for (unsigned int i=0; i<m_muonCollectionName.size(); i++) {	
	double m_eff_dimuon=0;
	double m_eff_pt=0;
	double m_eff_mass=0;    double m_reff_mass=0;
	double m_eff_elec=0;    double m_reff_elec=0;
	double m_eff_dphi=0;      double m_reff_dphi=0;
	double m_eff_z=0;
	
	if (m_Cut_Efficiency[i]->GetBinContent(1)>0) {
	  m_eff_dimuon=m_Cut_Flow[i]->GetBinContent(1)/m_Cut_Efficiency[i]->GetBinContent(1);
	  m_eff_z=m_Cut_Efficiency[i]->GetBinContent(2)/m_Cut_Efficiency[i]->GetBinContent(1);
	}
	if (m_Cut_Flow[i]->GetBinContent(1)>0) {
	  m_eff_pt=m_Cut_Flow[i]->GetBinContent(2)/m_Cut_Flow[i]->GetBinContent(1);
	  m_eff_mass=m_Cut_Flow[i]->GetBinContent(3)/m_Cut_Flow[i]->GetBinContent(1);
	  m_eff_elec=m_Cut_Flow[i]->GetBinContent(6)/m_Cut_Flow[i]->GetBinContent(1);
	  m_eff_dphi=m_Cut_Flow[i]->GetBinContent(7)/m_Cut_Flow[i]->GetBinContent(1);
	}
	if (m_Cut_Flow[i]->GetBinContent(2)>0) m_reff_mass=m_Cut_Flow[i]->GetBinContent(3)/m_Cut_Flow[i]->GetBinContent(2);
	if (m_Cut_Flow[i]->GetBinContent(5)>0) m_reff_elec=m_Cut_Flow[i]->GetBinContent(6)/m_Cut_Flow[i]->GetBinContent(5);
	if (m_Cut_Flow[i]->GetBinContent(6)>0) m_reff_dphi=m_Cut_Flow[i]->GetBinContent(7)/m_Cut_Flow[i]->GetBinContent(6);
	
	std::cout.precision(2);
	std::cout<<std::endl;
	std::cout<<"############################################################"<<std::endl;
	std::cout<<"#   "<<m_muonCollectionName[i]<<" - Cut efficiencies"<<std::endl;
	std::cout<<"# ----------------------------------- (absolut/relative)"<<std::endl;
	std::cout<<"############################################################"<<std::endl;
	std::cout<<"Dimuon events:     \t\t\t 1.00" <<std::endl;
	std::cout<<"Pt cut efficiency: \t\t\t "<< m_eff_pt <<std::endl;
	std::cout<<"Mass cut efficiency:\t\t\t "<< m_eff_mass <<"\t"<< m_reff_mass <<std::endl;
	std::cout<<"Electron cut efficiency:\t\t "<< m_eff_elec <<"\t"<<m_reff_elec<<std::endl;
	std::cout<<"DeltaPhi cut/Z efficiency: \t\t "<< m_eff_dphi <<"\t"<<m_reff_dphi<<std::endl;
	std::cout<<"------------------------------------------------------------"<<std::endl;
	std::cout<<"Overall dimuon efficiency: "<< m_eff_dimuon <<std::endl;
	std::cout<<"Overall Z efficiency:  "<< m_eff_z <<std::endl;
	std::cout<<"############################################################"<<std::endl;
	std::cout<<std::endl;	
      }
      
      //clear vectors
      if (m_doTruth) {
	m_True_NumberOfMuons.clear();
	m_True_ZMass.clear();
	m_True_DiMuonMass.clear();
	m_True_ZPt.clear();
	m_True_MuonPt.clear();
      }

      m_Reco_NumberOfTracks.clear();
      m_Reco_DiMuonPt.clear();
      m_Reco_PtMin.clear();
      m_Reco_PtMax.clear();
      m_Reco_DeltaPhi.clear();
      m_Reco_Charge.clear();
      m_Reco_MuonPt.clear();
      m_Reco_MuonEta.clear();

      m_Reco_DiMuonMass.clear();
      m_Reco_ZMass.clear();
  
      m_Cut_Efficiency.clear();
      m_Cut_Flow.clear();
      m_nCandidates_perEvent.clear();
      m_Reco_DeltaPhi_vs_Pt.clear();  
  
      m_Reco_JetEnergy_InHalo.clear();
      m_Reco_NumberOfTracks_InHalo.clear();
      m_Reco_TrackPt_InHalo.clear();
      m_Reco_ET_InHalo.clear();

      m_Reco_Mass_vs_Pt.clear();
      m_Reco_Mass_vs_Eta.clear();
      m_Reco_Mass_vs_Phi.clear();
      m_Reco_MassShift_vs_Pt.clear();
      m_Reco_MassShift_vs_Eta.clear();
      m_Reco_MassShift_vs_Phi.clear();

      /*
	m_h_pttotal->Delete();
	m_h_etatotal->Delete();
	m_h_phitotal->Delete();
	m_h_pttotal=0;
	m_h_etatotal=0;
	m_h_phitotal=0; 
      */
    } //end of run
  }//environment
  return StatusCode::SUCCESS;
} 
//-------------------------------------------------------------
StatusCode ZmumuPhysMonTool::finalize()
{
  ATH_MSG_DEBUG("finalize()");
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------

std::pair<double,double> ZmumuPhysMonTool::getTrackPropertiesInCone(const Rec::TrackParticle *track)
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
    if ( dR>m_isoConeMin && dR<m_isoConeMax && relPt>0.1) {
      nTracks++;
      sumPt+=(*trkItr)->pt();
    }
  }
  return std::make_pair(nTracks,sumPt);
}

double ZmumuPhysMonTool::getJetEnergyInCone(const Rec::TrackParticle *track)
{
  StatusCode sc = StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG( "in getJetEnergyInCone()");
  
  const JetCollection* mcpartJet;
  sc=m_storeGate->retrieve( mcpartJet, m_JetContainer);
  if( sc.isFailure() || !mcpartJet)  {
    ATH_MSG_WARNING( "Can't retrieve Jet container from StoreGate." ); 
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

double ZmumuPhysMonTool::getETIsolation(const Analysis::Muon *muon)
{
  ATH_MSG_DEBUG( "in getETIsolation()");
  return muon->parameter( MuonParameters::etcone40 );
}
bool ZmumuPhysMonTool::isElectron(const Rec::TrackParticle *track)
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
