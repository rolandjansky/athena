/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////
// Authors:  I.Nomidis (AUTh) - N. Benekos(Illinois)
// MuonDQA
// Feb. 2009
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include "MuonPhysicsMonitoring/JpsimumuPhysMonTool.h"
 
/////////////////////////////////////////////////////////////////////////////

JpsimumuPhysMonTool::JpsimumuPhysMonTool(const std::string& type, const std::string& name, const IInterface* parent) 
  : ManagedMonitorToolBase(type, name, parent) ,
    m_iVertexFitter("Trk::TrkVKalVrtFitter"),
    m_helpertool("InDet::ConversionFinderUtils"),
    m_vertexEstimator("InDet::VertexPointEstimator"),    
    m_V0Tools("Trk::V0Tools"),
    m_trigEffJpsiTools("TrigEffJpsiTools")
{
  declareProperty( "doMCTruth",                 m_doTruth = false);

  declareProperty( "InnerTrackContainer" ,      m_InnerTrackContainer="TrackParticleCandidate" );
  declareProperty( "MuonContainer",	        m_muonCollectionName);

  declareProperty( "muonDeltaRcut",	        m_cut_deltaR=0. );
  declareProperty( "diMuonMassWindow",	        m_cut_diMuonMass = 0);
  declareProperty( "muonHigherPt",              m_cut_muonHigherPt=0. );
  declareProperty( "muonLowerPt" ,              m_cut_muonLowerPt=0. ); //preselection pt-cut on muons
  declareProperty( "muonEtacut",                m_cut_muonEta=2.5 ); //preselection eta-cut on muons 
  declareProperty( "properTimeCut",             m_cut_properTime=0. ); //pseudo proper-time cut to select only direct Jpsi
  declareProperty( "vertexChi2Cut",             m_cut_JpsiVertexChi2 = 0);
  declareProperty( "lowerMassCut",              m_cut_lowerMass = 0);
  declareProperty( "upperMassCut",              m_cut_upperMass = 0);

  declareProperty( "jpsiMass",                  m_jpsiMass = 0);
  declareProperty( "jpsiMassBinning",           m_jpsiMassBinning=0.);

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
  declareProperty( "TriggerSelection",          m_trigSelection );
  declareProperty( "TrigEffJpsiTools", 		m_trigEffJpsiTools);
}
//==================================================================================
 

//======================================================================================//
/**  Initialize */
//======================================================================================//
StatusCode JpsimumuPhysMonTool::initialize() 
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
  IAlgTool *tmp_analysisTools;
  sc = toolSvc->retrieveTool("AnalysisTools",tmp_analysisTools);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR( "Can't get handle on analysis tools" );
    return StatusCode::FAILURE;
  }
  m_analysisTools=dynamic_cast<IAnalysisTools *>(tmp_analysisTools);

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

    //retrieve trigger matching tool
    if(m_trigEffJpsiTools.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve Tool " );
      return StatusCode::FAILURE;
    }else {
      ATH_MSG_INFO( "Got handle on TrigEffJpsiTools" );
      //      m_trigEffJpsiTools->initialize();
    }
  }

  nEvents = 0;

  return StatusCode::SUCCESS;
}

//==================================================================================

StatusCode JpsimumuPhysMonTool::bookHistogramsRecurrent()
{
  
  ATH_MSG_DEBUG( "in bookHistograms()" );

  if (newEventsBlock){}
  if (newLumiBlock){}
  if (newRun) {
    std::string m_generic_path_muonphysicsmonitoring = "Muon/MuonPhysicsMonitoring/Jpsimumu/";
  
    std::string path;
    for (unsigned int j=0; j<m_muonCollectionName.size(); j++) {
      StatusCode sc = StatusCode::SUCCESS;
      if (m_doTrigger && m_trigSelection!="") {
	path = m_generic_path_muonphysicsmonitoring+"/Overview/TriggerAware/"+m_muonCollectionName[j]+m_trigSelection+"/";
      }
      else path = m_generic_path_muonphysicsmonitoring+"/Overview/NoTrigger/"+m_muonCollectionName[j]+"/";
      
      
      //declare a group of histograms
      MonGroup muonphysmon_shift( this, path, run, ATTRIB_UNMANAGED );
      MonGroup muonphysmon_expert( this, path, run, ATTRIB_UNMANAGED);
      //define hists
            
      //Truth
      if(m_doTruth) {
	m_True_NumberOfMuons.push_back( new TH1F( "m_True_NumberOfMuons" , "Number of True Muons >6GeV" ,100,0,100));
	m_True_JpsiMass     .push_back( new TH1F( "m_True_JpsiMass" , "True J/#psi Mass [MeV]" ,100,0,10000));
	m_True_DiMuonMass   .push_back( new TH1F( "m_True_DiMuonMass" , "True DiMuon Mass [MeV]" ,100,0,6000));
	m_True_JpsiPt       .push_back( new TH1F( "m_True_JpsiPt" , "True J/#psi P_{T} [MeV]",100,2000,4000));
	m_True_MuonPt       .push_back( new TH1F( "m_True_Muon_Pt" , "True J/#psi Muon P_{T} [MeV]" , 100,0,10000));
	sc=muonphysmon_expert.regHist( m_True_NumberOfMuons.back());
	sc=muonphysmon_expert.regHist( m_True_JpsiMass.back());
	sc=muonphysmon_expert.regHist( m_True_DiMuonMass.back());
	sc=muonphysmon_expert.regHist( m_True_JpsiPt.back());
	sc=muonphysmon_expert.regHist( m_True_MuonPt.back());
      }
      
      int nBins = (m_cut_upperMass - m_cut_lowerMass)/m_jpsiMassBinning;

      m_Dimuon_Mass.push_back( new TH1F( "m_Dimuon_Mass" , "Dimuon invariant mass [MeV]" ,200,0,16000));
      m_Dimuon_MassAfterVtx.push_back( new TH1F( "m_Dimuon_MassAfterVtx" , "Dimuon invariant mass after vertexing [MeV]" , 200,0.,16000.));
      m_Dimuon_NCandidates.push_back( new TH1F( "m_Dimuon_NCandidates", "Dimuon candidates", 10, 0, 10));
      m_Dimuon_NCandidatesSameSign.push_back( new TH1F( "m_Dimuon_NCandidatesSameSign", "Dimuon candidates Same Sign", 10, 0, 10));
      m_Dimuon_PtMin          .push_back( new TH1F( "m_Dimuon_PtMin" , "Dimuon lower muon Pt [MeV]" , 80,0,40000));
      m_Dimuon_PtMax          .push_back( new TH1F( "m_Dimuon_PtMax" , "Dimuon higher muon Pt [MeV]" , 80,0,40000));
      m_Dimuon_PtMax_vs_PtMin .push_back( new TH2F( "m_Dimuon_PtMax_vs_PtMin" , "Dimuon higher vs lower muon Pt  [MeV]" , 80,0,40000,80,0,40000));
      m_Dimuon_Charge         .push_back( new TH1F( "m_Dimuon_Charge" , "Dimuon charge (q_{#mu 1} x q_{#mu 2})" , 3,-1.5,1.5));
      m_Dimuon_DeltaR       .push_back( new TH1F( "m_Dimuon_DeltaR" , "Dimuon #Delta R",50,0.,5.));
      m_Dimuon_VertexChi2.push_back( new TH1F("m_Dimuon_VertexChi2","Dimuon vertex #chi^{2}", 200,0,200 ));

      sc=muonphysmon_shift.regHist( m_Dimuon_Mass.back());
      sc=muonphysmon_shift.regHist( m_Dimuon_MassAfterVtx.back());
      sc=muonphysmon_expert.regHist( m_Dimuon_NCandidates.back());
      sc=muonphysmon_expert.regHist( m_Dimuon_NCandidatesSameSign.back());
      sc=muonphysmon_expert.regHist( m_Dimuon_PtMin.back());
      sc=muonphysmon_expert.regHist( m_Dimuon_PtMax.back());
      sc=muonphysmon_expert.regHist( m_Dimuon_PtMax_vs_PtMin.back());      
      sc=muonphysmon_expert.regHist( m_Dimuon_Charge.back());
      sc=muonphysmon_expert.regHist( m_Dimuon_DeltaR.back());
      sc=muonphysmon_expert.regHist( m_Dimuon_VertexChi2.back());
      

      m_Jpsi_Mass.push_back( new TH1F( "m_Jpsi_Mass" , "J/#psi invariant mass [MeV]" , nBins, m_cut_lowerMass,m_cut_upperMass));
      m_Jpsi_MassRefitted.push_back( new TH1F( "m_Jpsi_MassRefitted" , "J/#psi mass refitted [MeV]" , nBins, m_cut_lowerMass,m_cut_upperMass)); 
      m_Jpsi_MassSameSign.push_back( new TH1F( "m_Jpsi_MassSameSign" , "J/#psi invariant mass Same Sign [MeV]",nBins,m_cut_lowerMass,m_cut_upperMass)); 
      m_Jpsi_NCandidates.push_back( new TH1F( "m_Jpsi_NCandidates", "J/#psi candidates", 10, 0, 10));
      m_Jpsi_Propertime .push_back( new TH1F( "m_Jpsi_Propertime", "J/#psi pseudo-propertime [ps]",100,-2.5,2.5));
      m_Jpsi_Pt       .push_back( new TH1F( "m_Jpsi_Pt" , "J/#psi p_{T} [MeV]", 80,0,40000));
      m_Jpsi_DeltaR         .push_back( new TH1F( "m_Jpsi_DeltaR" , "J/#psi #Delta R",50,0.,5.));
      m_Jpsi_DeltaR_vs_Pt   .push_back( new TH2F( "m_Jpsi_DeltaR_vs_pt","J/#psi #Delta R vs p_{T} [MeV]", 100,0,10000.,50,0.,5.));
      m_Jpsi_PtMax_vs_PtMin .push_back( new TH2F( "m_Jpsi_PtMax_vs_PtMin" , "J#psi higher vs lower muon Pt  [MeV]" , 80,0,40000,80,0,40000));
      m_Jpsi_VertexChi2.push_back( new TH1F("m_Jpsi_VertexChi2","J/#psi vertex #chi^{2}", 200,0,200 ));
      sc=muonphysmon_shift.regHist( m_Jpsi_Mass.back());
      sc=muonphysmon_expert.regHist( m_Jpsi_MassRefitted.back());
      sc=muonphysmon_expert.regHist( m_Jpsi_MassSameSign.back());
      sc=muonphysmon_expert.regHist( m_Jpsi_NCandidates.back());
      sc=muonphysmon_shift.regHist( m_Jpsi_Propertime.back());
      sc=muonphysmon_expert.regHist( m_Jpsi_Pt.back());
      sc=muonphysmon_expert.regHist( m_Jpsi_DeltaR.back());
      sc=muonphysmon_expert.regHist( m_Jpsi_DeltaR_vs_Pt.back());
      sc=muonphysmon_expert.regHist( m_Jpsi_PtMax_vs_PtMin.back());
      sc=muonphysmon_shift.regHist( m_Jpsi_VertexChi2.back());
 



      
      m_Jpsi_Mass_BB.push_back( new TH1F( "m_Jpsi_Mass_BB" , "J/#psi mass in Barrel [MeV]", nBins,m_cut_lowerMass,m_cut_upperMass));
      m_Jpsi_Mass_BEA.push_back( new TH1F( "m_Jpsi_Mass_BEA" , "J/#psi mass in Barrel-Endcap A [MeV]", nBins,m_cut_lowerMass,m_cut_upperMass));
      m_Jpsi_Mass_BEC.push_back( new TH1F( "m_Jpsi_Mass_BEC" , "J/#psi mass in Barrel-Endcap C [MeV]", nBins,m_cut_lowerMass,m_cut_upperMass));
      m_Jpsi_Mass_EEA.push_back( new TH1F( "m_Jpsi_Mass_EEA" , "J/#psi mass in Endcap A [MeV]", nBins,m_cut_lowerMass,m_cut_upperMass));
      m_Jpsi_Mass_EEC.push_back( new TH1F( "m_Jpsi_Mass_EEC" , "J/#psi mass in Endcap C [MeV]", nBins,m_cut_lowerMass,m_cut_upperMass));

      m_Jpsi_Mass_per_Region.push_back( new TProfile( "m_Jpsi_Mass_per_Region" , "J/#psi mass per detector region [MeV]", 1,0,1));
      sc=muonphysmon_shift.regHist( m_Jpsi_Mass_BB.back() );
      sc=muonphysmon_shift.regHist( m_Jpsi_Mass_BEA.back() );
      sc=muonphysmon_shift.regHist( m_Jpsi_Mass_BEC.back() );
      sc=muonphysmon_shift.regHist( m_Jpsi_Mass_EEA.back() );
      sc=muonphysmon_shift.regHist( m_Jpsi_Mass_EEC.back() );
      sc=muonphysmon_shift.regHist( m_Jpsi_Mass_per_Region.back() );

      m_Jpsi_Mass_CC.push_back( new TH1F( "m_Jpsi_Mass_CC", "J/#psi mass from Combined-Combined Muons [MeV]", nBins,m_cut_lowerMass,m_cut_upperMass));
      m_Jpsi_Mass_CT.push_back( new TH1F( "m_Jpsi_Mass_CT", "J/#psi mass from Combined-Tagged Muons [MeV]", nBins,m_cut_lowerMass,m_cut_upperMass));
      m_Jpsi_Mass_TT.push_back( new TH1F( "m_Jpsi_Mass_TT", "J/#psi mass from Tagged-Tagged Muons [MeV]", nBins,m_cut_lowerMass,m_cut_upperMass));
      sc=muonphysmon_shift.regHist( m_Jpsi_Mass_CC.back() );
      sc=muonphysmon_expert.regHist( m_Jpsi_Mass_CT.back() );
      sc=muonphysmon_expert.regHist( m_Jpsi_Mass_TT.back() );

            
      m_Jpsi_Mass_vs_Pt.push_back( new TProfile("m_Jpsi_Mass_vs_Pt","J/#psi mass profile vs p_{T} [MeV]",m_binsPt.size()-1 , &m_binsPt[0] )) ;
      m_Jpsi_Mass_vs_Eta.push_back( new TProfile("m_Jpsi_Mass_vs_Eta","J/#psi mass profile vs #eta", m_binsEta.size()-1 , &m_binsEta[0] )) ;
      m_Jpsi_Mass_vs_Phi.push_back( new TProfile("m_Jpsi_Mass_vs_Phi","J/#psi mass profile vs #phi", m_binsPhi.size()-1 , &m_binsPhi[0] )) ;
      m_Jpsi_MassShift_vs_Pt.push_back( new TH1F("m_Jpsi_MassShift_vs_Pt","J/#psi mass shift vs p_{T} [MeV]",m_binsPt.size()-1 , &m_binsPt[0] )) ;
      m_Jpsi_MassShift_vs_Eta.push_back( new TH1F("m_Jpsi_MassShift_vs_Eta","J/#psi mass shift vs #eta", m_binsEta.size()-1 , &m_binsEta[0] )) ;
      m_Jpsi_MassShift_vs_Phi.push_back( new TH1F("m_Jpsi_MassShift_vs_Phi","J/#psi mass shift vs #phi", m_binsPhi.size()-1 , &m_binsPhi[0] )) ;
      sc=muonphysmon_shift.regHist(m_Jpsi_Mass_vs_Pt.back());
      sc=muonphysmon_shift.regHist(m_Jpsi_Mass_vs_Eta.back());
      sc=muonphysmon_shift.regHist( m_Jpsi_Mass_vs_Phi.back());
      sc=muonphysmon_shift.regHist(m_Jpsi_MassShift_vs_Pt.back());
      sc=muonphysmon_shift.regHist(m_Jpsi_MassShift_vs_Eta.back());
      sc=muonphysmon_shift.regHist( m_Jpsi_MassShift_vs_Phi.back());
      

      m_Muon_NCandidates .push_back( new TH1F( "m_Muon_NCandidates" , "Reconstructed muons" , 10,0,10));
      m_Muon_Pt        .push_back( new TH1F( "m_Muon_Pt", "Reconstructed muon p_{T} [MeV]", 100,0,100000));
      m_Muon_Eta       .push_back( new TH1F( "m_Muon_Eta", "Reconstructed muon #eta", 54, -2.7, 2.7));
      m_Muon_Pt_vs_Eta.push_back( new TH2F("m_Muon_Pt_vs_Eta","Reconstructed muon p_{T} vs #eta", 54,-2.5,2.5,100,0,100000 ));
      m_Muon_Type_vs_Eta.push_back( new TH2F("m_Muon_Type_vs_Eta","Reconstructed muon type vs #eta", 54,-2.5,2.5, 1,0,1 ));
      m_Muon_Type_vs_Pt.push_back( new TH2F("m_Muon_Type_vs_Pt","Reconstructed muon type vs p_{T}", 100,0,100000, 1,0,1 ));
      sc=muonphysmon_expert.regHist( m_Muon_NCandidates.back());
      sc=muonphysmon_expert.regHist( m_Muon_Pt.back());
      sc=muonphysmon_expert.regHist( m_Muon_Eta.back());           
      sc=muonphysmon_expert.regHist( m_Muon_Pt_vs_Eta.back());
      sc=muonphysmon_expert.regHist( m_Muon_Type_vs_Eta.back());
      sc=muonphysmon_expert.regHist( m_Muon_Type_vs_Pt.back());

      m_Cut_Efficiency      .push_back( new TH1F( "m_Cut_Efficiency" , "Number of Events (1:AllEvents, 2:J/#psi candidates, 3:J/#psi matched with truth, 4:Fake J/#psi)" , 6,0.5,6.5));
      m_Cut_Flow            .push_back( new TH1F( "m_Cut_Flow","Cut flow (1:#mu^{+}#mu^{-}, 2:Vtx, 3:#chi^{2}, 4:p_{T}, 5:#tau, 6:!electron, 7:#Delta R, 8:Mass)" , 9,0.5,9.5));
      sc=muonphysmon_expert.regHist( m_Cut_Efficiency.back());
      sc=muonphysmon_shift.regHist( m_Cut_Flow.back());

      if (sc.isFailure()) {
	ATH_MSG_FATAL("Failed to register histogram(s)");
	return sc;
      }
    } //muonCollection loop---------

  } //NewRun
    
  return StatusCode::SUCCESS;
}
//================================================================================== 
StatusCode JpsimumuPhysMonTool::fillHistograms()
{  
  ATH_MSG_DEBUG( "in fillHistograms()" );

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
    StatusCode sc = m_storeGate->retrieve(importedMuonContainer, m_muonCollectionName[i] );
    if (sc.isFailure() || !importedMuonContainer) {
      ATH_MSG_WARNING( "No MuonCollection " << m_muonCollectionName[i] << " found in StoreGate" );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG( "Found "<< importedMuonContainer->size() << " muons in container" );
        
    m_Cut_Efficiency[i]->Fill(1); //number of all events
    std::vector<const Analysis::Muon*> myMuons;
    std::vector<const Rec::TrackParticle*> myMuonTracks;
    Analysis::MuonContainer::const_iterator muItr;
    
    // Loop inside muon collection, keep only combined and low-pt muons, apply preselection cut on pt and eta
    for ( muItr=importedMuonContainer->begin(); muItr!=importedMuonContainer->end(); muItr++) {
      
      if ( !(*muItr)->hasInDetTrackParticle() ) {
	ATH_MSG_DEBUG( "Muon has not ID track. Skipping" );
	continue;
      }
      std::string type="";      
      if( (*muItr)->isCombinedMuon() ) type="combined";
      else type="tagged";
      if (type!="combined" && type!="tagged")  continue;
      
      const Rec::TrackParticle* muTrk =0; 
      muTrk = (*muItr)->inDetTrackParticle();
      if (muTrk==0) continue;
      
      m_Muon_Pt[i]->Fill( muTrk->pt() );
      m_Muon_Eta[i]->Fill( muTrk->eta() );
      m_Muon_Pt_vs_Eta[i]->Fill( muTrk->eta(), muTrk->pt(),1 );
      m_Muon_Type_vs_Eta[i]->Fill( muTrk->eta(), type.c_str(), 1 );
      m_Muon_Type_vs_Pt[i]->Fill( muTrk->pt(), type.c_str(), 1 );
      if ( muTrk->pt()>m_cut_muonLowerPt ) {
	if ( muTrk->eta()<m_cut_muonEta ) {
	  myMuons.push_back((*muItr));
	  myMuonTracks.push_back(muTrk);
	}
      }
    }
    
    m_Muon_NCandidates[i]->Fill(myMuonTracks.size());
    ATH_MSG_DEBUG( "Got "<< myMuonTracks.size() << " muons after preselection" );
    
    if (myMuonTracks.size()<2) {
      ATH_MSG_DEBUG( "Can't make pair. Skipping event..." );
      return StatusCode::SUCCESS;
    }
    
    //make pairs of oppositely charged muons
    double charge1=0.;
    double charge2=0.;
    std::vector< std::pair<const Analysis::Muon*,const Analysis::Muon*> > myMuonPairs;
    std::vector< std::pair<const Rec::TrackParticle*,const Rec::TrackParticle*> > myMuonTrackPairs;
    std::vector< std::pair<const Analysis::Muon*,const Analysis::Muon*> > myMuonPairsSS;
    std::vector< std::pair<const Rec::TrackParticle*,const Rec::TrackParticle*> > myMuonTrackPairsSS;

    for (unsigned int imuon=0; imuon<myMuonTracks.size()-1; imuon++) {
      charge1 = myMuonTracks[imuon]->charge();
      ATH_MSG_DEBUG( "Got muon1 with q=" <<charge1 );
      for (unsigned int jmuon=imuon+1; jmuon<myMuonTracks.size(); jmuon++) {
	charge2=myMuonTracks[jmuon]->charge();
	ATH_MSG_DEBUG( "Got muon2 with q=" <<charge2 );
	
	//throw away tagged-tagged muon combination
	if ( !myMuons[imuon]->isCombinedMuon() && !myMuons[jmuon]->isCombinedMuon()) continue; 

	m_Dimuon_Charge[i]->Fill(charge1*charge2);
	if (charge1*charge2<0) {
	  ATH_MSG_DEBUG( "Got pair of oppositely charged muons" );

	  //sort muons according to pt, the one with the higher pt goes first
	  if (myMuonTracks[imuon]->pt() > myMuonTracks[jmuon]->pt()) {
	    myMuonTrackPairs.push_back(std::make_pair(myMuonTracks[imuon],myMuonTracks[jmuon]));
	    myMuonPairs.push_back(std::make_pair(myMuons[imuon],myMuons[jmuon]));
	  }
	  else {
	    myMuonTrackPairs.push_back(std::make_pair(myMuonTracks[jmuon],myMuonTracks[imuon]));
	    myMuonPairs.push_back(std::make_pair(myMuons[jmuon],myMuons[imuon]));
	  }
	}
	else {
	  ATH_MSG_DEBUG( "Got pair of same sign muons" );
	  
	  //sort muons according to pt, the one with the higher pt goes first
	  if (myMuonTracks[imuon]->pt() > myMuonTracks[jmuon]->pt()) {
	    myMuonTrackPairsSS.push_back(std::make_pair(myMuonTracks[imuon],myMuonTracks[jmuon]));
	    myMuonPairsSS.push_back(std::make_pair(myMuons[imuon],myMuons[jmuon]));
	  }
	  else {
	    myMuonTrackPairsSS.push_back(std::make_pair(myMuonTracks[jmuon],myMuonTracks[imuon]));
	    myMuonPairsSS.push_back(std::make_pair(myMuons[jmuon],myMuons[imuon]));
	  }

	}
      }
    }
    ATH_MSG_DEBUG( "Found " << myMuonTrackPairs.size() << " muon pairs" );
    ATH_MSG_DEBUG( "Found " << myMuonTrackPairsSS.size() << " SS muon pairs" );
    
    m_Dimuon_NCandidates[i]->Fill(myMuonTrackPairs.size());
    m_Dimuon_NCandidatesSameSign[i]->Fill(myMuonTrackPairsSS.size());
    //if (m_doTrigger) m_numDimuons_per_TriggerItem->Fill(m_trigSelection[i].c_str(),myMuonPairs.size());
  
    if (myMuonTrackPairs.size()==0 && myMuonTrackPairsSS.size()==0 ) {
      ATH_MSG_DEBUG( "No pairs, skipping event..." );
      return StatusCode::SUCCESS;
    }
    
    //loop over muon pairs, apply cuts and find J/psi candidates
    int nJpsi=0;
    std::vector< std::pair<const Rec::TrackParticle*,const Rec::TrackParticle*> > myJpsiMuonTracks;
    std::vector< std::pair<const Analysis::Muon*,const Analysis::Muon*> > myJpsiMuons;

    for (unsigned int j=0; j<myMuonTrackPairs.size(); j++) {
      ATH_MSG_DEBUG( "Getting pair #"<< (j+1) );
      
      const Analysis::Muon* muon1 = myMuonPairs[j].first;
      const Analysis::Muon* muon2 = myMuonPairs[j].second;
      const Rec::TrackParticle* muonTrack1 = myMuonTrackPairs[j].first;
      const Rec::TrackParticle* muonTrack2 = myMuonTrackPairs[j].second;

      if (!muon1 || !muon2) {
	ATH_MSG_DEBUG( "Null muon, skipping pair" );
	continue;
      }
      
      double pt1 = muonTrack1->pt();
      double pt2 = muonTrack2->pt();
      double pt = ( muonTrack1->hlv() + muonTrack2->hlv() ).perp();
      double deltaR = m_analysisTools->deltaR(muonTrack1,muonTrack2);
      double invMass = CorrectInvMassTwoMuons(muonTrack1, muonTrack2); //resolution in low-pt region is better for the Inner Detector rather than the Muon Spectometer

      ATH_MSG_DEBUG( "Dimuon mass: " << invMass );      

      m_Dimuon_Mass[i]->Fill(invMass);
      
      //quantities after vertexing
      double refMass=-999;
      //double Lxy=-999.;
      double vtxChi2=-999.;
      double tau=-999.;
      
      //fill histograms for muon pairs, before cuts
      m_Cut_Flow[i]->Fill(1);
      
      //apply cuts
      ATH_MSG_DEBUG( "Applying cuts..." );

      if (doVertexing(muonTrack1, muonTrack2, vtxChi2, refMass, tau) || m_doVertexing==false) {
	m_Cut_Flow[i]->Fill(2);
	ATH_MSG_DEBUG( "Successful vertexing...: Chi2: "<<vtxChi2<<", refMass:"<< refMass<<", tau:" << tau);
	m_Dimuon_MassAfterVtx[i]->Fill(invMass);
	m_Dimuon_DeltaR[i]->Fill(deltaR);
	m_Dimuon_PtMax[i]->Fill(pt1);
	m_Dimuon_PtMin[i]->Fill(pt2);
	m_Dimuon_PtMax_vs_PtMin[i]->Fill(pt2,pt1);
	m_Dimuon_VertexChi2[i]->Fill(vtxChi2);
	if ( vtxChi2<=m_cut_JpsiVertexChi2 || m_cut_JpsiVertexChi2==0 ) { 
	  m_Cut_Flow[i]->Fill(3);	    
	  if (pt1>m_cut_muonHigherPt && pt2>m_cut_muonLowerPt) {
	    m_Cut_Flow[i]->Fill(4);	    
	    if( (tau<=m_cut_properTime) || (m_cut_properTime==0) ) {
	      m_Cut_Flow[i]->Fill(5);
	      if ( !isElectron(muonTrack1) && !isElectron(muonTrack2) ) {
		m_Cut_Flow[i]->Fill(6);
		if ( deltaR<=m_cut_deltaR || m_cut_deltaR==0 ) {
		  m_Cut_Flow[i]->Fill(7);
		  //if ( (fabs(invMass-m_jpsiMass)<m_cut_diMuonMass) || (m_cut_diMuonMass==0) ) {
		  if ( invMass>m_cut_lowerMass && invMass<m_cut_upperMass ) {
		    ATH_MSG_DEBUG( "Got a J/psi candidate" );
		    m_Cut_Flow[i]->Fill(8);
		    m_Cut_Efficiency[i]->Fill(2);
		    nJpsi++;

		    m_Jpsi_Mass[i]->Fill(invMass);
		    m_Jpsi_MassRefitted[i]->Fill(refMass);
		    m_Jpsi_Pt[i]->Fill(pt);
		    m_Jpsi_DeltaR[i]->Fill(deltaR);
		    m_Jpsi_DeltaR_vs_Pt[i]->Fill(pt,deltaR);
		    m_Jpsi_PtMax_vs_PtMin[i]->Fill(pt2,pt1);
		    m_Jpsi_VertexChi2[i]->Fill(vtxChi2);
		    m_Jpsi_Propertime[i]->Fill(tau);

		    m_Jpsi_Mass_vs_Pt[i]  -> Fill(pt, invMass); 
		    m_Jpsi_Mass_vs_Eta[i] -> Fill((muonTrack1->hlv()+muonTrack2->hlv()).pseudoRapidity(), invMass);
		    m_Jpsi_Mass_vs_Phi[i] -> Fill((muonTrack1->hlv()+muonTrack2->hlv()).phi(), invMass);


		    myJpsiMuonTracks.push_back(std::make_pair(muonTrack1,muonTrack2));
		    myJpsiMuons.push_back(std::make_pair(muon1,muon2));
		  }
		}
	      }
	    }
	  }
	}  
      }
    }//close for
    
    m_Jpsi_NCandidates[i]->Fill(nJpsi);
    if (nJpsi>1) m_Cut_Efficiency[i]->Fill(4);
    //if (m_doTrigger)    m_numJpsi_per_TriggerItem->Fill(m_trigSelection[i].c_str(),nJpsi);
  
    if (myJpsiMuonTracks.size()==0) {
      ATH_MSG_DEBUG( "No J/psi candidates" );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_INFO( "Found " << nJpsi << " J/psi candidates" );
    
    for (unsigned int j=0; j<myJpsiMuonTracks.size(); j++) {
      double invMass = CorrectInvMassTwoMuons(myJpsiMuonTracks[j].first,myJpsiMuonTracks[j].second); 
      double Eta1= myJpsiMuonTracks[j].first->eta();   double Eta2= myJpsiMuonTracks[j].second->eta();

      bool isCombinedCombined=false;
      bool isCombinedTagged=false;
      bool isTaggedTagged=false;
      if (myJpsiMuons[j].first->isCombinedMuon() && myJpsiMuons[j].second->isCombinedMuon()) isCombinedCombined=true;
      else if ( (myJpsiMuons[j].first->isCombinedMuon() && !myJpsiMuons[j].second->isCombinedMuon())  || (myJpsiMuons[j].second->isCombinedMuon() && !myJpsiMuons[j].first->isCombinedMuon()) ) isCombinedTagged=true;
      else if (!myJpsiMuons[j].first->isCombinedMuon() && !myJpsiMuons[j].second->isCombinedMuon()) isTaggedTagged=true;
      
      //J/psi mass asking at least one combined muon
      if (isCombinedCombined) {
	m_Jpsi_Mass_CC[i]->Fill(invMass);
      }
      else if (isCombinedTagged) {
	m_Jpsi_Mass_CT[i]->Fill(invMass);
      }
      else if (isTaggedTagged) {
	m_Jpsi_Mass_TT[i]->Fill(invMass);
      }

      bool isBarrelBarrel=false;
      bool isBarrelEndcapA=false;
      bool isBarrelEndcapC=false;
      bool isEndcapEndcapA=false;
      bool isEndcapEndcapC=false;

      if      (fabs(Eta1)<=1.4 && fabs(Eta2)<=1.4)            	        isBarrelBarrel=true;
      else if ( (Eta1<1.4 && Eta2>1.4) || (Eta2<1.4 && Eta1>1.4) )	isBarrelEndcapA=true;
      else if ( (Eta1<-1.4 && Eta2>-1.4) || (Eta2<-1.4 && Eta1>-1.4) )	isBarrelEndcapC=true;
      else if (Eta1>1.4 && Eta2>1.4)	                                isEndcapEndcapA=true;
      else if (Eta1<=-1.4 && Eta2<=-1.4)                                isEndcapEndcapC=true;
      else ATH_MSG_WARNING("Could not determine region of Jpsi muon");
      
      if (isBarrelBarrel) {
	m_Jpsi_Mass_BB[i]->Fill(invMass);
	m_Jpsi_Mass_per_Region[i]->Fill("BB",invMass);
      } 
      else if (isBarrelEndcapA) {
	m_Jpsi_Mass_BEA[i]->Fill(invMass);
	m_Jpsi_Mass_per_Region[i]->Fill("BEA",invMass);
      }
      else if (isBarrelEndcapC) {
	m_Jpsi_Mass_BEC[i]->Fill(invMass);
	m_Jpsi_Mass_per_Region[i]->Fill("BEC",invMass);
      }
      else if (isEndcapEndcapA) {
	m_Jpsi_Mass_EEA[i]->Fill(invMass);
	m_Jpsi_Mass_per_Region[i]->Fill("EEA",invMass);
      }
      else if (isEndcapEndcapC) {
	m_Jpsi_Mass_EEC[i]->Fill(invMass);
	m_Jpsi_Mass_per_Region[i]->Fill("EEC",invMass);
      }

      //Trigger efficiency
      /*
	if (m_doTrigger && m_doJpsigerEfficiency) {
	ATH_MSG_DEBUG( "Counting events for trigger efficiency" );
	if (i==0) { //noTriggerSelection
	m_h_etatotal->Fill(eta, 1.);
	m_h_pttotal->Fill(invPt, 1.);
	m_h_phitotal->Fill(phi, 1.);
	}
	else {
	//checking if this event was triggered
	if ( m_trigDecTool->isPassed(m_trigSelection[i]) ) { 
	//checking if it was one of the J/psi muons that fired the trigger
	if ( m_trigEffJpsiTools->isTriggeredMuon( myJpsiMuonTracks[i].first->inDetTrackParticle(),1) || 
	m_trigEffJpsiTools->isTriggeredMuon( myJpsiMuonTracks[i].second->inDetTrackParticle(),1) ) {
	m_h_etatrig[i-1]->Fill(eta, 1.);
	m_h_pttrig[i-1]->Fill(invPt, 1.);
	m_h_phitrig[i-1]->Fill(phi, 1.);
	}
	}
	}
	}
      */
    }

    //same sign muon analysis_________________________
    ATH_MSG_DEBUG( "SS muon analysis" );
    for (unsigned int j=0; j<myMuonTrackPairsSS.size(); j++) {
      ATH_MSG_DEBUG( "Getting pair #"<< (j+1) );
      
      const Analysis::Muon* muon1 = myMuonPairsSS[j].first;
      const Analysis::Muon* muon2 = myMuonPairsSS[j].second;
      const Rec::TrackParticle* muonTrack1 = myMuonTrackPairsSS[j].first;
      const Rec::TrackParticle* muonTrack2 = myMuonTrackPairsSS[j].second;

      if (!muon1 || !muon2) {
	ATH_MSG_DEBUG( "Null muon, skipping pair" );
	continue;
      }
      
      double pt1 = muonTrack1->pt();
      double pt2 = muonTrack2->pt();
      double deltaR = m_analysisTools->deltaR(muonTrack1,muonTrack2);
      double invMass = CorrectInvMassTwoMuons(muonTrack1, muonTrack2);
      
      //quantities after vertexing
      double refMass=-999;
      double vtxChi2=-999.;
      double tau=-999.;
            
      //apply cuts
      ATH_MSG_DEBUG( "Applying cuts..." );
      if (doVertexing(muonTrack1, muonTrack2, vtxChi2, refMass, tau) || m_doVertexing==false) {
	ATH_MSG_DEBUG( "Successful vertexing...: Chi2: "<<vtxChi2<<", refMass:"<< refMass<<", tau:" << tau);
	if ( vtxChi2<=m_cut_JpsiVertexChi2 || m_cut_JpsiVertexChi2==0 ) { 
	  if (pt1>m_cut_muonHigherPt && pt2>m_cut_muonLowerPt) {	    
	    if( (tau<=m_cut_properTime) || (m_cut_properTime==0) ) {
	      if ( !isElectron(muonTrack1) && !isElectron(muonTrack2) ) {
		if ( deltaR<=m_cut_deltaR || m_cut_deltaR==0 ) {
		  if ( invMass>m_cut_lowerMass && invMass<m_cut_upperMass ) {
		    ATH_MSG_DEBUG( "SS muon pair passed all J/psi selection cuts" );
		    m_Jpsi_MassSameSign[i]->Fill(invMass);
		  }
		}
	      }
	    }
	  }
	}  
      }
    }//close for

  }
  return StatusCode::SUCCESS;
}

//==================================================================================
StatusCode JpsimumuPhysMonTool::procHistograms()
{ 
  ATH_MSG_DEBUG( "procHistograms()" );

  if(endOfEventsBlock){}
  if(endOfLumiBlock){}
  if(endOfRun){

    /*
      if (m_doTrigger) {
      m_numJpsi_per_TriggerItem->LabelsDeflate("X");
      m_numDimuons_per_TriggerItem->LabelsDeflate("X");
      }
    */

    for (unsigned int i=0; i<m_muonCollectionName.size(); i++) {
      
      for (int ibin=1; ibin<=m_Jpsi_Mass_vs_Pt[i]->GetNbinsX(); ibin++) {
	double dMassShift = m_Jpsi_Mass_vs_Pt[i]->GetBinContent(ibin) - 3096.9;
	m_Jpsi_MassShift_vs_Pt[i]   -> Fill(m_binsPt[ibin-1],  dMassShift);
      }
      m_Jpsi_MassShift_vs_Pt[i]->SetMarkerStyle(2);
      m_Jpsi_MassShift_vs_Pt[i]->SetOption("P");
      
      for (int ibin=1; ibin<=m_Jpsi_MassShift_vs_Eta[i]->GetNbinsX(); ibin++) {
	double dMassShift = m_Jpsi_Mass_vs_Eta[i]->GetBinContent(ibin) - 3096.9;
	m_Jpsi_MassShift_vs_Eta[i]   -> Fill(m_binsEta[ibin-1],  dMassShift);
      }
      m_Jpsi_MassShift_vs_Eta[i]->SetMarkerStyle(2);
      m_Jpsi_MassShift_vs_Eta[i]->SetOption("P");
      
      for (int ibin=1; ibin<=m_Jpsi_MassShift_vs_Phi[i]->GetNbinsX(); ibin++) {
	double dMassShift = m_Jpsi_Mass_vs_Phi[i]->GetBinContent(ibin) - 3096.9;
	m_Jpsi_MassShift_vs_Phi[i] -> Fill(m_binsPhi[ibin-1],  dMassShift);
      }
      m_Jpsi_MassShift_vs_Phi[i]->SetMarkerStyle(2);
      m_Jpsi_MassShift_vs_Phi[i]->SetOption("P");
    }
    //J/psi trigger Efficiencies
    /*
      if (m_doTrigger && m_doJpsigerEfficiency && m_trigSelection.size()>1 && i>0) {	
      for (int ibin=1; ibin<=m_Jpsi_TrigEfficiency_Eta[i-1]->GetNbinsX(); ibin++) {
      if (m_h_etatotal->GetBinContent(ibin)>0) {
      m_Jpsi_TrigEfficiency_Eta[i-1]->Fill(m_binsEta[ibin-1], m_h_etatrig[i-1]->GetBinContent(ibin) / m_h_etatotal->GetBinContent(ibin));
      }
      }
      m_Jpsi_TrigEfficiency_Eta[i-1]->SetMarkerStyle(2);
      m_Jpsi_TrigEfficiency_Eta[i-1]->SetOption("P");
      m_Jpsi_TrigEfficiency_Eta[i-1]->GetYaxis()->SetRangeUser(0.,1.1);
	
      for (int ibin=1; ibin<=m_Jpsi_TrigEfficiency_Phi[i-1]->GetNbinsX(); ibin++) {
      if (m_h_phitotal->GetBinContent(ibin)>0) {
      m_Jpsi_TrigEfficiency_Phi[i-1]->Fill(m_binsPhi[ibin-1], m_h_phitrig[i-1]->GetBinContent(ibin) / m_h_phitotal->GetBinContent(ibin));
      }
      }
      m_Jpsi_TrigEfficiency_Phi[i-1]->SetMarkerStyle(2);
      m_Jpsi_TrigEfficiency_Phi[i-1]->SetOption("P");
      m_Jpsi_TrigEfficiency_Phi[i-1]->GetYaxis()->SetRangeUser(0.,1.1);

      for (int ibin=1; ibin<=m_Jpsi_TrigEfficiency_Pt[i-1]->GetNbinsX(); ibin++) {
      if (m_h_pttotal->GetBinContent(ibin)>0) {
      m_Jpsi_TrigEfficiency_Pt[i-1]->Fill(m_binsPt[ibin-1], m_h_pttrig[i-1]->GetBinContent(ibin) / m_h_pttotal->GetBinContent(ibin));
      }
      }
      m_Jpsi_TrigEfficiency_Pt[i-1]->SetMarkerStyle(2);
      m_Jpsi_TrigEfficiency_Pt[i-1]->SetOption("P");
      m_Jpsi_TrigEfficiency_Pt[i-1]->GetYaxis()->SetRangeUser(0.,1.1);
	
      m_h_pttrig[i-1]->Delete();
      m_h_etatrig[i-1]->Delete();
      m_h_phitrig[i-1]->Delete();
      m_h_pttrig[i-1]=0;
      m_h_etatrig[i-1]=0;
      m_h_phitrig[i-1]=0;
      }   

      //print relative and absolut efficincies
      double m_eff_dimuon=0;
      double m_eff_pt=0;
      double m_eff_mass=0;    double m_reff_mass=0;
      double m_eff_vtx=0;     double m_reff_vtx=0;
      double m_eff_proptime=0;double m_reff_proptime=0;
      double m_eff_elec=0;    double m_reff_elec=0;
      double m_eff_dr=0;      double m_reff_dr=0;
      double m_eff_jpsi=0;
      
      if (m_Cut_Efficiency[i]->GetBinContent(1)>0) {
      m_eff_dimuon=m_Cut_Flow[i]->GetBinContent(1)/m_Cut_Efficiency[i]->GetBinContent(1);
      m_eff_jpsi=m_Cut_Efficiency[i]->GetBinContent(2)/m_Cut_Efficiency[i]->GetBinContent(1);
      }
      if (m_Cut_Flow[i]->GetBinContent(1)>0) {
      m_eff_pt=m_Cut_Flow[i]->GetBinContent(2)/m_Cut_Flow[i]->GetBinContent(1);
      m_eff_mass=m_Cut_Flow[i]->GetBinContent(3)/m_Cut_Flow[i]->GetBinContent(1);
      m_eff_vtx=m_Cut_Flow[i]->GetBinContent(4)/m_Cut_Flow[i]->GetBinContent(1);
      m_eff_proptime=m_Cut_Flow[i]->GetBinContent(5)/m_Cut_Flow[i]->GetBinContent(1);
      m_eff_elec=m_Cut_Flow[i]->GetBinContent(6)/m_Cut_Flow[i]->GetBinContent(1);
      m_eff_dr=m_Cut_Flow[i]->GetBinContent(7)/m_Cut_Flow[i]->GetBinContent(1);
      }
      if (m_Cut_Flow[i]->GetBinContent(2)>0) m_reff_mass=m_Cut_Flow[i]->GetBinContent(3)/m_Cut_Flow[i]->GetBinContent(2);
      if (m_Cut_Flow[i]->GetBinContent(3)>0) m_reff_vtx=m_Cut_Flow[i]->GetBinContent(4)/m_Cut_Flow[i]->GetBinContent(3);
      if (m_Cut_Flow[i]->GetBinContent(4)>0) m_reff_proptime=m_Cut_Flow[i]->GetBinContent(5)/m_Cut_Flow[i]->GetBinContent(4);
      if (m_Cut_Flow[i]->GetBinContent(5)>0) m_reff_elec=m_Cut_Flow[i]->GetBinContent(6)/m_Cut_Flow[i]->GetBinContent(5);
      if (m_Cut_Flow[i]->GetBinContent(6)>0) m_reff_dr=m_Cut_Flow[i]->GetBinContent(7)/m_Cut_Flow[i]->GetBinContent(6);

      std::cout.precision(2);
      std::cout<<std::endl;
      std::cout<<"############################################################"<<std::endl;
      std::cout<<"#   "<<m_trigSelection[i]<<" - Cut efficiencies"<<std::endl;
      std::cout<<"# ----------------------------------- (absolut/relative)"<<std::endl;
      std::cout<<"############################################################"<<std::endl;
      std::cout<<"Dimuon events:     \t\t\t 1.00" <<std::endl;
      std::cout<<"Pt cut efficiency: \t\t\t "<< m_eff_pt <<std::endl;
      std::cout<<"Mass cut efficiency:\t\t\t "<< m_eff_mass <<"\t"<< m_reff_mass <<std::endl;
      std::cout<<"Vrtx cut efficiency:\t\t\t "<< m_eff_vtx <<"\t"<< m_reff_vtx << std::endl;
      std::cout<<"Pr.time cut efficiency:\t\t\t "<< m_eff_proptime<<"\t"<<m_reff_proptime <<std::endl;
      std::cout<<"Electron cut efficiency:\t\t "<< m_eff_elec <<"\t"<<m_reff_elec<<std::endl;
      std::cout<<"DeltaR cut/Jpsi efficiency: \t\t "<< m_eff_dr <<"\t"<<m_reff_dr<<std::endl;
      std::cout<<"------------------------------------------------------------"<<std::endl;
      std::cout<<"Overall dimuon efficiency: "<< m_eff_dimuon <<std::endl;
      std::cout<<"Overall J/psi efficiency:  "<< m_eff_jpsi <<std::endl;
      std::cout<<"############################################################"<<std::endl;
      std::cout<<std::endl;
      }
    */

    if(m_doTruth) {
      m_True_NumberOfMuons.clear();
      m_True_JpsiMass.clear();
      m_True_DiMuonMass.clear();
      m_True_JpsiPt.clear();
      m_True_MuonPt.clear();
    }

    m_Muon_NCandidates.clear();
    m_Muon_Pt.clear();
    m_Muon_Eta.clear();
    m_Muon_Pt_vs_Eta.clear();
    m_Muon_Type_vs_Eta.clear();
    m_Muon_Type_vs_Pt.clear();

    m_Dimuon_Mass.clear();
    m_Dimuon_MassAfterVtx.clear();
    m_Dimuon_PtMin.clear();
    m_Dimuon_PtMax.clear();
    m_Dimuon_PtMax_vs_PtMin.clear();
    m_Dimuon_DeltaR.clear();
    m_Dimuon_Charge.clear();
    m_Dimuon_VertexChi2.clear();
    m_Dimuon_NCandidates.clear();
    m_Dimuon_NCandidatesSameSign.clear();

    m_Jpsi_MassRefitted.clear();
    m_Jpsi_Mass.clear();
    m_Jpsi_MassSameSign.clear();
    m_Jpsi_Propertime.clear();
    m_Jpsi_Pt.clear();
    m_Jpsi_VertexChi2.clear();
    m_Jpsi_DeltaR.clear();
    m_Jpsi_DeltaR_vs_Pt.clear();
    m_Jpsi_PtMax_vs_PtMin.clear();  
    m_Jpsi_NCandidates.clear();
  
    m_Jpsi_Mass_vs_Pt.clear();
    m_Jpsi_Mass_vs_Eta.clear();
    m_Jpsi_Mass_vs_Phi.clear();

    m_Jpsi_MassShift_vs_Pt.clear();
    m_Jpsi_MassShift_vs_Eta.clear();
    m_Jpsi_MassShift_vs_Phi.clear();

    m_Jpsi_Mass_BB.clear();
    m_Jpsi_Mass_BEA.clear();
    m_Jpsi_Mass_BEC.clear();
    m_Jpsi_Mass_EEA.clear();
    m_Jpsi_Mass_EEC.clear();
    m_Jpsi_Mass_per_Region.clear();

    m_Jpsi_Mass_CC.clear();
    m_Jpsi_Mass_CT.clear();
    m_Jpsi_Mass_TT.clear();

    m_Cut_Efficiency.clear();
    m_Cut_Flow.clear();

    /*
      m_Trig_Cut_Flow.clear();
      m_numJpsi_per_TriggerItem.clear();
      m_numDimuons_per_TriggerItem.clear();
    */
    if (m_doTrigger) {
      if (m_trigEffJpsiTools->finalize().isFailure()) {
	ATH_MSG_WARNING("Failed to finalize TrigEffJpsiTool");
      }
    }
    
  }
  return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////
StatusCode JpsimumuPhysMonTool::finalize()
{
  return StatusCode::SUCCESS;
}
/////////////////////////////////////////////////////////
bool JpsimumuPhysMonTool::isElectron(const Rec::TrackParticle *track)
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

bool JpsimumuPhysMonTool::doVertexing(const Rec::TrackParticle *theTag, const Rec::TrackParticle *theProbe, double &vtxChi2, double &mass, double &tau)
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

  // retrieve the vertex container from StoreGate
  const VxContainer *importedVxContainer;
  if (evtStore()->contains<VxContainer>("VxPrimaryCandidate")) {    
    if( m_storeGate->retrieve(importedVxContainer,"VxPrimaryCandidate").isFailure() ) {
      ATH_MSG_WARNING( "Can't retrieve vertex container from StoreGate." ); 
      //return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_DEBUG("StoreGate doesn't contain primary vertex container");
    return StatusCode::SUCCESS;
  }

  // Check that you got at least one primary vertex from StoreGate  
  if( importedVxContainer->size() == 0 ) {
    ATH_MSG_WARNING( "You have lack of primary vertices." );
  }
  // Check that the primary vertex covariance matrix is non-singular
  Trk::RecVertex *primaryVertex;
  primaryVertex = &( ( *(importedVxContainer->begin()) ) -> recVertex() );
  if (!primaryVertex) ATH_MSG_ERROR( "Couldn't find primary vertex" ); 
  
 // double determinant = primaryVertex->errorPosition().covariance().determinant();
 // if( determinant == 0 ) {
 //   ATH_MSG_WARNING( "Primary vertex with zero determinant found." );
 //   primaryVertex = NULL;
 // }
  
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
    const double mytau = m_V0Tools->tau(&(*myExVxCandidate),(*primaryVertex),3096.9);
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
