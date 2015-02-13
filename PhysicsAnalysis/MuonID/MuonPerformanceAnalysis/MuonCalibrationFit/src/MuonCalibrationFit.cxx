/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibrationFit/MuonCalibrationFit.h"

MuonCalibrationFit::MuonCalibrationFit( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ) {
  //::: General properties
  declareProperty( "BatchMode", m_BatchMode = kTRUE );
  declareProperty( "InputForID", m_InputForID = "" );
  declareProperty( "Input", m_Input = "input.root" );
  declareProperty( "Output", m_Output = "output.root" );
  declareProperty( "InputRegionsFileForID", m_InputRegionsFileForID = "" );
  declareProperty( "InputRegionsFile", m_InputRegionsFile = "TestSingleRegion.txt" );
  declareProperty( "OutputRegionsFile", m_OutputRegionsFile = "TestSingleRegionOut.txt" );
  declareProperty( "DataFile", m_DataFile = "Data.root" );
  declareProperty( "BackgroundFile", m_BackgroundFile = "Background.root" );
  declareProperty( "MonteCarloFile", m_MonteCarloFile = "MonteCarlo.root" );
  declareProperty( "MaxEvents", m_MaxEvents = -1 );
  //::: Fitting properties
  declareProperty( "Detector", m_Detector = "ID" );
  declareProperty( "FitType", m_FitType = "Chi2" );
  declareProperty( "BkgParametrization", m_Bkg = "Exponential" );
  declareProperty( "RunMode", m_RunMode = "Simplex:1" );
  declareProperty( "MaxRetrials", m_MaxRetrials = 3 );
  declareProperty( "ProbThreshold", m_ProbThreshold = 0.1 );
  //::: Parameters' properties
  declareProperty( "S0_Init", m_Init[ Par::s0 ] = 0. ); //::: ???
  declareProperty( "S0_Step", m_Step[ Par::s0 ] = 0.01 );
  declareProperty( "S1_Init", m_Init[ Par::s1 ] = 0. ); //::: -0.001
  declareProperty( "S1_Step", m_Step[ Par::s1 ] = 0.001 );
  declareProperty( "P0_Init", m_Init[ Par::p0 ] = 0. ); //::: ???
  declareProperty( "P0_Step", m_Step[ Par::p0 ] = 0.1 );
  declareProperty( "P1_Init", m_Init[ Par::p1 ] = 0. ); //::: 0.01
  declareProperty( "P1_Step", m_Step[ Par::p1 ] = 0.003 );
  declareProperty( "P2_Init", m_Init[ Par::p2 ] = 0. ); //::: 0.0005
  declareProperty( "P2_Step", m_Step[ Par::p2 ] = 0.001 );
  declareProperty( "Use_S0", m_Use[ Par::s0 ] = kFALSE );
  declareProperty( "Use_S1", m_Use[ Par::s1 ] = kFALSE );
  declareProperty( "Use_P0", m_Use[ Par::p0 ] = kFALSE );
  declareProperty( "Use_P1", m_Use[ Par::p1 ] = kFALSE );
  declareProperty( "Use_P2", m_Use[ Par::p2 ] = kFALSE );
  //::: Templates' properties
  declareProperty( "Splitting", m_Splitting = 1 );
  declareProperty( "NumRandomValues", m_NumRandomValues = 1 );
  declareProperty( "JpsiMass_Bins", m_JpsiMass_Bins = 80 );
  declareProperty( "JpsiMass_Min", m_JpsiMass_Min = 2.7 );
  declareProperty( "JpsiMass_Max", m_JpsiMass_Max = 3.5 );
  declareProperty( "JpsiMass_Pts", m_JpsiMass_Pts = "" );
  declareProperty( "ZMass_Bins", m_ZMass_Bins = 50 );
  declareProperty( "ZMass_Min", m_ZMass_Min = 66 );
  declareProperty( "ZMass_Max", m_ZMass_Max = 116 );
  declareProperty( "ZMass_Pts", m_ZMass_Pts = "" );
  declareProperty( "ZRho_Bins", m_ZRho_Bins = 50 );
  declareProperty( "ZRho_Min", m_ZRho_Min = -0.2 );
  declareProperty( "ZRho_Max", m_ZRho_Max = 0.2 );
  declareProperty( "ZRho_Pts", m_ZRho_Pts = "" );
}

StatusCode MuonCalibrationFit::initialize() {
  ATH_MSG_INFO( "initializing" );
  ATH_MSG_DEBUG( "Input: " << m_Input );
  ATH_MSG_DEBUG( "InputForID: " << m_InputForID );
  ATH_MSG_DEBUG( "Output: " << m_Output );
  ATH_MSG_DEBUG( "InputRegionsFile: " << m_InputRegionsFile );
  ATH_MSG_DEBUG( "InputRegionsFileForID: " << m_InputRegionsFileForID );
  ATH_MSG_DEBUG( "OutputRegionsFile: " << m_OutputRegionsFile );
  ATH_MSG_DEBUG( "DataFile: " << m_DataFile );
  ATH_MSG_DEBUG( "BackgroundFile: " << m_BackgroundFile );
  ATH_MSG_DEBUG( "MonteCarloFile: " << m_MonteCarloFile );
  ATH_MSG_DEBUG( "MaxEvents: " << m_MaxEvents );
  ATH_MSG_DEBUG( "Detector: " << m_Detector );
  ATH_MSG_DEBUG( "FitType: " << m_FitType );
  ATH_MSG_DEBUG( "Bkg: " << m_Bkg );
  ATH_MSG_DEBUG( "RunMode: " << m_RunMode );
  ATH_MSG_DEBUG( "ProbThreshold: " << m_ProbThreshold );
  ATH_MSG_DEBUG( "S0_Fit: " << m_Use[ Par::s0 ] );
  ATH_MSG_DEBUG( "S0_Init: " << m_Init[ Par::s0 ] );
  ATH_MSG_DEBUG( "S0_Step: " << m_Step[ Par::s0 ] );
  ATH_MSG_DEBUG( "S1_Fit: " << m_Use[ Par::s1 ] );
  ATH_MSG_DEBUG( "S1_Init: " << m_Init[ Par::s1 ] );
  ATH_MSG_DEBUG( "S1_Step: " << m_Step[ Par::s1 ] );
  ATH_MSG_DEBUG( "P0_Fit: " << m_Use[ Par::p0 ] );
  ATH_MSG_DEBUG( "P0_Init: " << m_Init[ Par::p0 ] );
  ATH_MSG_DEBUG( "P0_Step: " << m_Step[ Par::p0 ] );
  ATH_MSG_DEBUG( "P1_Fit: " << m_Use[ Par::p1 ] );
  ATH_MSG_DEBUG( "P1_Init: " << m_Init[ Par::p1 ] );
  ATH_MSG_DEBUG( "P1_Step: " << m_Step[ Par::p1 ] );
  ATH_MSG_DEBUG( "P2_Fit: " << m_Use[ Par::p2 ] );
  ATH_MSG_DEBUG( "P2_Init: " << m_Init[ Par::p2 ] );
  ATH_MSG_DEBUG( "P2_Step: " << m_Step[ Par::p2 ] );
  ATH_MSG_DEBUG( "Splitting: " << m_Splitting );
  ATH_MSG_DEBUG( "NumRandomValues: " << m_NumRandomValues );
  ATH_MSG_DEBUG( "JpsiMass_Bins: " << m_JpsiMass_Bins );
  ATH_MSG_DEBUG( "JpsiMass_Min: " << m_JpsiMass_Min );
  ATH_MSG_DEBUG( "JpsiMass_Max: " << m_JpsiMass_Max );
  ATH_MSG_DEBUG( "JpsiMass_Pts: " << m_JpsiMass_Pts );
  ATH_MSG_DEBUG( "ZMass_Bins: " << m_ZMass_Bins );
  ATH_MSG_DEBUG( "ZMass_Min: " << m_ZMass_Min );
  ATH_MSG_DEBUG( "ZMass_Max: " << m_ZMass_Max );
  ATH_MSG_DEBUG( "ZMass_Pts: " << m_ZMass_Pts );
  //:::
  RooMsgService::instance().setGlobalKillBelow( RooFit::WARNING );
  RooMsgService::instance().setSilentMode( kTRUE ); 
  gErrorIgnoreLevel = kWarning;
  //:::
  g_DataSingleInfo = new std::vector< Tools::Info >;
  g_DataBothInfo = new std::vector< Tools::Info >;
  g_BackgroundSingleInfo = new std::vector< Tools::Info >;
  g_BackgroundBothInfo = new std::vector< Tools::Info >;
  g_MonteCarloSingleInfo = new std::vector< Tools::Info >;
  g_MonteCarloBothInfo = new std::vector< Tools::Info >; 
  //:::
  g_Output = new TFile( m_Output.c_str(), "recreate" );
  g_Index = 0;
  g_UpdatedTemplates = 0;
  //:::
  if( m_FitType == "Chi2" ) {
    m_DoChi2 = kTRUE;
    m_DoNLL = kFALSE;
  }
  else if( m_FitType == "Likelihood" ) {
    m_DoChi2 = kFALSE;
    m_DoNLL = kTRUE;
  }
  if( m_DoChi2 ) ATH_MSG_INFO( "Running chi2 fit" );
  else if( m_DoNLL ) ATH_MSG_INFO( "Running nll fit" );
  else {
    ATH_MSG_ERROR( "Invalid initialization, unknown combination of options: Chi2 = " << m_DoChi2 << ", Nll = " << m_DoNLL );
    return StatusCode::FAILURE;
  }
  //:::
  m_Error = ( m_DoChi2 ) ? 1. : 0.5;
  //:::
  m_ParNames[ Par::s0 ] = "s0"; m_PosDef[ Par::s0 ] = kFALSE;
  m_ParNames[ Par::s1 ] = "s1"; m_PosDef[ Par::s1 ] = kFALSE;
  m_ParNames[ Par::p0 ] = "p0"; m_PosDef[ Par::p0 ] = kTRUE;
  m_ParNames[ Par::p1 ] = "p1"; m_PosDef[ Par::p1 ] = kTRUE;
  m_ParNames[ Par::p2 ] = "p2"; m_PosDef[ Par::p2 ] = kTRUE;
 
  m_MinInit[ Par::s0 ] = -0.1;  m_MaxInit[ Par::s0 ] = 0.1;  
  m_MinInit[ Par::s1 ] = -0.01; m_MaxInit[ Par::s1 ] = 0.01; 
  m_MinInit[ Par::p0 ] = 0.;    m_MaxInit[ Par::p0 ] = 0.1;    
  m_MinInit[ Par::p1 ] = 0.;    m_MaxInit[ Par::p1 ] = 0.1;    
  m_MinInit[ Par::p2 ] = 0.;    m_MaxInit[ Par::p2 ] = 0.001;    

  m_MinStep[ Par::s0 ] = 0.01;    m_MaxStep[ Par::s0 ] = 0.1;  
  m_MinStep[ Par::s1 ] = 0.0001;  m_MaxStep[ Par::s1 ] = 0.01; 
  m_MinStep[ Par::p0 ] = 0.01;    m_MaxStep[ Par::p0 ] = 0.1;    
  m_MinStep[ Par::p1 ] = 0.001;   m_MaxStep[ Par::p1 ] = 0.1;    
  m_MinStep[ Par::p2 ] = 0.00001; m_MaxStep[ Par::p2 ] = 0.01;    
  //:::
  if( m_Detector == "ID" ) {
    //::: Some overrides, just to be 110% sure...
    m_Use[ Par::s0 ] = kFALSE;
    m_Use[ Par::p0 ] = kFALSE;
  }
  //:::
  return StatusCode::SUCCESS;
}

StatusCode MuonCalibrationFit::execute() {
  ATH_MSG_INFO( "executing" );
  Regions* InReg = new Regions( m_InputRegionsFile, m_Input, kFALSE );
  Regions* InRegForID = NULL;
  if( m_InputRegionsFileForID != "" && m_InputForID != "" ) InRegForID = new Regions( m_InputRegionsFileForID, m_InputForID, kFALSE );
  Regions* OutReg = new Regions( m_OutputRegionsFile, "", kFALSE );
  //::: Loop on Regions
  std::vector< Tools::RegionInfo* > InRegInfos = InReg->GetRegions();
  std::vector< Tools::RegionInfo* > InRegInfosForID;
  if( InRegForID != NULL ) InRegInfosForID = InRegForID->GetRegions();
  std::vector< Tools::RegionInfo* > OutRegInfos = OutReg->GetRegions();
  for( std::vector< Tools::RegionInfo* >::iterator It = OutRegInfos.begin(); It != OutRegInfos.end(); ++It ) {
    //::: 
    for( int index = 0; index < Par::N; index++ ) {
      g_pars[ index ] = -1.; 
      g_errs_up[ index ] = -1.; 
      g_errs_low[ index ] = -1.; 
    }
    g_NLL = -1.;
    g_Chi2 = -1.;
    g_ScanIter = 1;
    g_ScanPar = -1;
    g_FitTrial = 0;
    g_FilledData = kFALSE;
    ( *It )->Print();
    g_Output->cd();
    g_Tree = new TTree( "tree_" + ( ( *It )->Name ), "" );
    g_Tree->Branch( "iteration", &g_Index,           "iteration/I" );
    g_Tree->Branch( "s0",        &g_pars[ Par::s0 ], "s0/D"        );
    g_Tree->Branch( "s1",        &g_pars[ Par::s1 ], "s1/D"        );
    g_Tree->Branch( "p0",        &g_pars[ Par::p0 ], "p0/D"        );
    g_Tree->Branch( "p1",        &g_pars[ Par::p1 ], "p1/D"        );
    g_Tree->Branch( "p2",        &g_pars[ Par::p2 ], "p2/D"        );
    g_Tree->Branch( "nll",       &g_NLL,             "nll/D"       );
    g_Tree->Branch( "chi2",      &g_Chi2,            "chi2/D"      );
    g_Tree->Branch( "scan_par",  &g_ScanPar,         "scan_par/I"  );
    g_Tree->Branch( "scan_iter", &g_ScanIter,        "scan_iter/I" );
    //::: 
    g_CurrentTemplate = new Template( ( *It )->Name, ( *It )->Name, m_Splitting, m_Bkg );
    if( m_JpsiMass_Pts != "" ) g_CurrentTemplate->AddMassJpsi( m_JpsiMass_Bins, m_JpsiMass_Min, m_JpsiMass_Max, GetVector( m_JpsiMass_Pts ) );
    if( m_ZMass_Pts    != "" ) {
      g_CurrentTemplate->AddMassZ( m_ZMass_Bins, m_ZMass_Min, m_ZMass_Max, GetVector( m_ZMass_Pts ) );
    }
    if( m_JpsiMass_Pts != "" && m_ZMass_Pts != "" ) {
      g_CurrentTemplate->AddMonitoredVariable( "Eta", "#eta", "#eta", 50, -2.5, 2.5 );
      g_CurrentTemplate->AddMonitoredVariable( "Pt", "p_{T}", "p_{T} [GeV]", 44, 6, 50, 57, 6, 120 );
      g_CurrentTemplate->AddMonitoredVariable( "Phi", "#phi", "#phi", 50, -TMath::Pi(), TMath::Pi() );
      g_CurrentTemplate->AddMonitoredVariable( "DeltaPhi", "#Delta #phi", "#Delta #phi", 50, 0, TMath::Pi() );
      g_CurrentTemplate->AddMonitoredVariable( "DeltaEta", "#Delta #eta", "#Delta #eta", 50, 0, 5 );
      g_CurrentTemplate->AddMonitoredVariable( "PairPt", "p_{T}^{#mu#mu} [GeV]", "p_{T}^{#mu#mu} [GeV]", 50, 0, 50 );
      g_CurrentTemplate->AddMonitoredVariable( "PairEta", "#eta^{#mu#mu}", "#eta^{#mu#mu}", 50, -5, 5 );
      g_CurrentTemplate->AddMonitoredVariable( "PairPhi", "#phi^{#mu#mu}", "#phi^{#mu#mu}", 50, -TMath::Pi(), TMath::Pi() );
    }
    if( m_ZRho_Pts     != "" ) g_CurrentTemplate->AddRhoZ( m_ZRho_Bins, m_ZRho_Min, m_ZRho_Max, GetVector( m_ZRho_Pts ) );
    g_Output->cd();
    FillVectors( InRegInfos, InRegInfosForID, *It );
    //::://::://::: Truth part 
    float AverageEta = ( *It )->GetEta();
    if( AverageEta < -100 ) ATH_MSG_WARNING( "Unable to retrieve meaningful average eta!!!" ); 
    const double true_pars[ Par::N ] = { GetTruth( AverageEta, Par::s0 ), GetTruth( AverageEta, Par::s1 ), GetTruth( AverageEta, Par::p0 ), GetTruth( AverageEta, Par::p1 ), GetTruth( AverageEta, Par::p2 ) };
    const double true_errors[ Par::N ] = { 0, 0, 0, 0, 0 };
    GetFunction( true_pars );
    DoMonitoringPlots( true_pars );
    g_CurrentTemplate->Write( "TrueFit" ); 
    SaveParameters( "TrueFit", true_pars, true_errors );
    g_CurrentTemplate->Print();
    //::: Set Initial parameters based on true ones!
    for( int par_index = 0; par_index < Par::N; par_index++ ) {
      m_Init[ par_index ] = true_pars[ par_index ];
    }
    //::://::://::: PreFit part 
    // const double init_pars[ Par::N ] = { m_Init[ Par::s0 ], m_Init[ Par::s1 ], m_Init[ Par::p0 ], m_Init[ Par::p1 ], m_Init[ Par::p2 ] };
    // const double init_errors[ Par::N ] = { 0, 0, 0, 0, 0 };
    // GetFunction( init_pars );
    // DoMonitoringPlots( init_pars );
    // g_CurrentTemplate->Write( "PreFit" ); 
    // SaveParameters( "PreFit", init_pars, init_errors );
    //::://::://::: Extracting Run Mode Information
    boost::char_separator< char > BigSep( "+" );
    boost::char_separator< char > SmallSep( ":" );
    boost::tokenizer< boost::char_separator< char > > BigTokens( m_RunMode, BigSep );
    bool converged = kFALSE;
    int n_retrials = 0;
    while( ! converged && n_retrials++ < m_MaxRetrials ) {
      g_ScanPar = -1;
      for( boost::tokenizer< boost::char_separator< char > >::iterator BigToken = BigTokens.begin(); BigToken != BigTokens.end(); ++BigToken ) {
        boost::tokenizer< boost::char_separator< char > > SmallTokens( *BigToken, SmallSep );
        std::string algo_name = "";
        int max_retrials = 1;
        int index = 0;
        for( boost::tokenizer< boost::char_separator< char > >::iterator SmallToken = SmallTokens.begin(); SmallToken != SmallTokens.end(); ++SmallToken ) {
          if( index == 0 ) algo_name = *SmallToken; 
          if( index == 1 ) max_retrials = atoi( ( *SmallToken ).c_str() ); 
          index++;
        }
        converged = RunMinimizationStep( algo_name, max_retrials );
      }
    }
    //::://::://:::
    Stop( *It );
    ATH_MSG_DEBUG( ( *It )->GetInfo() );
  }
  g_Output->Close();
  delete InReg;
  delete OutReg;
  return StatusCode::SUCCESS;
}

StatusCode MuonCalibrationFit::finalize() {
  ATH_MSG_INFO( "finalizing" );
  return StatusCode::SUCCESS;
}

double MuonCalibrationFit::GetNegLogLikelihood( const double* par ) {
  UpdateTemplates( par );
  double result = - g_CurrentTemplate->GetLogLikelihood( m_NumRandomValues );
  g_NLL = double( result );  
  g_Tree->Fill();
  ATH_MSG_VERBOSE( " ---> nll = " << std::setw( 10 ) << g_NLL << " | s0 = " << std::setw( 10 ) << g_pars[ Par::s0 ] << " | s1 = " << std::setw( 10 ) << g_pars[ Par::s1 ] << " | p0 = " << std::setw( 10 ) << g_pars[ Par::p0 ] << " | p1 = " << std::setw( 10 ) << g_pars[ Par::p1 ] << " | p2 = " << std::setw( 10 ) << g_pars[ Par::p2 ] ); 
  return result; 
}

double MuonCalibrationFit::GetChiSquare( const double* par ) {
  UpdateTemplates( par );
  double result = g_CurrentTemplate->GetChiSquare( m_NumRandomValues );
  g_Chi2 = double( result );  
  g_Tree->Fill();
  ATH_MSG_VERBOSE( " ---> chi2 = " << std::setw( 10 ) << g_Chi2 << " | s0 = " << std::setw( 10 ) << g_pars[ Par::s0 ] << " | s1 = " << std::setw( 10 ) << g_pars[ Par::s1 ] << " | p0 = " << std::setw( 10 ) << g_pars[ Par::p0 ] << " | p1 = " << std::setw( 10 ) << g_pars[ Par::p1 ] << " | p2 = " << std::setw( 10 ) << g_pars[ Par::p2 ] << " | scanIter = " << std::setw( 10 ) << g_ScanIter << " | scanPar = " << std::setw( 10 ) << g_ScanPar ); 
  return result; 
}

void MuonCalibrationFit::DoMonitoringPlots( const double* par ) {
  //::: Data
  std::vector< std::string > names = g_CurrentTemplate->GetMonitoredVariables();
  BOOST_FOREACH( Tools::Info theInfo, *g_DataSingleInfo ) {
    for( auto name: names ) {
      g_CurrentTemplate->FillMonitoredVariable( Type::Data, name, theInfo.GetFirst( name ), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ), theInfo.GetMass(), theInfo.IsOS, 1, true ); 
      g_CurrentTemplate->FillMonitoredVariable( Type::Data, name, theInfo.GetSecond( name ), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ), theInfo.GetMass(), theInfo.IsOS, 1, false ); 
    }
  }
  BOOST_FOREACH( Tools::Info theInfo, *g_DataBothInfo ) {
    for( auto name: names ) {
      g_CurrentTemplate->FillMonitoredVariable( Type::Data, name, theInfo.GetFirst( name ), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ), theInfo.GetMass(), theInfo.IsOS, 1, true ); 
      g_CurrentTemplate->FillMonitoredVariable( Type::Data, name, theInfo.GetSecond( name ), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ), theInfo.GetMass(), theInfo.IsOS, 1, true ); 
    }
  }
  //::: Background
  BOOST_FOREACH( Tools::Info theInfo, *g_BackgroundSingleInfo ) {
    if( ! theInfo.IsOS ) continue;
    for( auto name: names ) {
      g_CurrentTemplate->FillMonitoredVariable( Type::Background, name, theInfo.GetFirst( name ), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ), theInfo.GetMass(), theInfo.IsOS, theInfo.Weight, true ); 
      g_CurrentTemplate->FillMonitoredVariable( Type::Background, name, theInfo.GetSecond( name ), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ), theInfo.GetMass(), theInfo.IsOS, theInfo.Weight, false ); 
    }
  }
  BOOST_FOREACH( Tools::Info theInfo, *g_BackgroundBothInfo ) {
    if( ! theInfo.IsOS ) continue;
    for( auto name: names ) {
      g_CurrentTemplate->FillMonitoredVariable( Type::Background, name, theInfo.GetFirst( name ), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ), theInfo.GetMass(), theInfo.IsOS, theInfo.Weight, true ); 
      g_CurrentTemplate->FillMonitoredVariable( Type::Background, name, theInfo.GetSecond( name ), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ), theInfo.GetMass(), theInfo.IsOS, theInfo.Weight, true ); 
    }
  }
  //::: MonteCarlo
  //::: Single Muon Correction
  TRandom2 rand_gen( 19101985 );
  BOOST_FOREACH( Tools::Info theInfo, *g_MonteCarloSingleInfo ) {
    if( ! theInfo.IsOS ) continue;
    double rnd = rand_gen.Gaus( 0, 1 );
    for( auto name: names ) {
      g_CurrentTemplate->FillMonitoredVariable( Type::MC, name, theInfo.GetFirst( name, par, rnd ), std::max( theInfo.Corr_First_Mu_pt( par, rnd ), theInfo.Second_Mu_pt ), theInfo.GetMassSingle( par, rnd ), theInfo.IsOS, theInfo.Weight, true ); 
      g_CurrentTemplate->FillMonitoredVariable( Type::MC, name, theInfo.GetSecond( name ), std::max( theInfo.Corr_First_Mu_pt( par, rnd ), theInfo.Second_Mu_pt ), theInfo.GetMassSingle( par, rnd ), theInfo.IsOS, theInfo.Weight, false ); 
    }
  }
  //::: Double Muon Correction
  BOOST_FOREACH( Tools::Info theInfo, *g_MonteCarloBothInfo ) {
    if( ! theInfo.IsOS ) continue;
    double rnd1 = rand_gen.Gaus( 0, 1 );
    double rnd2 = rand_gen.Gaus( 0, 1 );
    for( auto name: names ) {
      g_CurrentTemplate->FillMonitoredVariable( Type::MC, name, theInfo.GetFirst( name, par, rnd1, rnd2 ), std::max( theInfo.Corr_First_Mu_pt( par, rnd1 ), theInfo.Corr_Second_Mu_pt( par, rnd2 ) ), theInfo.GetMassBoth( par, rnd1, rnd2 ), theInfo.IsOS, theInfo.Weight, true ); 
      g_CurrentTemplate->FillMonitoredVariable( Type::MC, name, theInfo.GetSecond( name, par, rnd2 ), std::max( theInfo.Corr_First_Mu_pt( par, rnd1 ), theInfo.Corr_Second_Mu_pt( par, rnd2 ) ), theInfo.GetMassBoth( par, rnd1, rnd2 ), theInfo.IsOS, theInfo.Weight, true ); 
    }
  }
  g_CurrentTemplate->MonitoredPlotsAreEmpty( false );
}

void MuonCalibrationFit::FillVectors( std::vector< Tools::RegionInfo* > RegInfos, std::vector< Tools::RegionInfo* > RegInfosForID, Tools::RegionInfo* RegInfo ) {
  //::: Data 
  g_DataSingleInfo->clear();
  g_DataBothInfo->clear();
  TChain chainData( "muon" );
  chainData.Add( m_DataFile.c_str() ); 
  Long_t MaxEvents = ( m_MaxEvents < 0 ) ? chainData.GetEntries() : m_MaxEvents;
  Reader readerData( Type::Data, MaxEvents, ( m_Detector == "ID" ), !m_BatchMode, RegInfos, RegInfosForID, RegInfo, g_DataSingleInfo, g_DataBothInfo );
  chainData.Process( &readerData, "", MaxEvents, 0 );
  //::: Background 
  g_BackgroundSingleInfo->clear();
  g_BackgroundBothInfo->clear();
  TChain chainBackground( "muon" );
  chainBackground.Add( m_BackgroundFile.c_str() ); 
  MaxEvents = ( m_MaxEvents < 0 ) ? chainBackground.GetEntries() : m_MaxEvents;
  Reader readerBackground( Type::Background, MaxEvents, ( m_Detector == "ID" ), !m_BatchMode, RegInfos, RegInfosForID, RegInfo, g_BackgroundSingleInfo, g_BackgroundBothInfo );
  chainBackground.Process( &readerBackground, "", MaxEvents, 0 );
  //::: MonteCarlo 
  g_MonteCarloSingleInfo->clear();
  g_MonteCarloBothInfo->clear();
  TChain chainMonteCarlo( "muon" );
  chainMonteCarlo.Add( m_MonteCarloFile.c_str() ); 
  MaxEvents = ( m_MaxEvents < 0 ) ? chainMonteCarlo.GetEntries() : m_MaxEvents;
  Reader readerMonteCarlo( Type::MC, MaxEvents, ( m_Detector == "ID" ), !m_BatchMode, RegInfos, RegInfosForID, RegInfo, g_MonteCarloSingleInfo, g_MonteCarloBothInfo );
  chainMonteCarlo.Process( &readerMonteCarlo, "", MaxEvents, 0 );
  std::cout << std::endl;
  ATH_MSG_INFO( "Size of    data    vector with 1 object  in region: " << g_DataSingleInfo->size() );
  ATH_MSG_INFO( "Size of    data    vector with 2 objects in region: " << g_DataBothInfo->size() );
  ATH_MSG_INFO( "Size of background vector with 1 object  in region: " << g_BackgroundSingleInfo->size() );
  ATH_MSG_INFO( "Size of background vector with 2 objects in region: " << g_BackgroundBothInfo->size() );
  ATH_MSG_INFO( "Size of montecarlo vector with 1 object  in region: " << g_MonteCarloSingleInfo->size() );
  ATH_MSG_INFO( "Size of montecarlo vector with 2 objects in region: " << g_MonteCarloBothInfo->size() );
}

std::vector< float >* MuonCalibrationFit::GetVector( TString all_values ) { 
  TObjArray* Array = all_values.Tokenize( " " );
  std::vector< float >* res = new std::vector< float >;
  for( int i = 0 ; i < Array->GetEntries(); ++i ) {
    TString TempString = Array->At( i )->GetName();
    if( TempString != "" ) {
      res->push_back( TempString.Atof() );
    }
  }
  return res;
}

void MuonCalibrationFit::SaveTime( TString name, float time ) {
  TVectorD temp_vector( 1 );
  temp_vector[ 0 ] = time;
  TString template_name = g_CurrentTemplate->GetName();
  temp_vector.Write( name + "___" + template_name );
}

void MuonCalibrationFit::SaveParameters( TString name, const double* pars, const double* errs_up, const double* errs_low ) {
  TVectorD temp_vector( 3 * Par::N + 2 ); // For each parameter save value, error+, error-, likelihood and chi2
  for( int index = 0; index < Par::N; index++ ) {
    if( m_PosDef[ index ] && pars[ index ] < 0 ) {
      temp_vector[ 3 * index ]     =  - pars[ index ];
      temp_vector[ 3 * index + 1 ] = errs_low[ index ];
      temp_vector[ 3 * index + 2 ] = errs_up[ index ];
    }
    else {
      temp_vector[ 3 * index ]     = pars[ index ];
      temp_vector[ 3 * index + 1 ] = errs_up[ index ];
      temp_vector[ 3 * index + 2 ] = errs_low[ index ];
    }
  }
  temp_vector[ 3 * Par::N ]     = g_NLL;
  temp_vector[ 3 * Par::N + 1 ] = g_Chi2;
  TString template_name = g_CurrentTemplate->GetName();
  temp_vector.Write( name + "___" + template_name );
  if( name == "PreFit" || name == "FirstEstimate" || name == "TrueFit" ) {
    ATH_MSG_INFO( "  " << name << "  ---> nll = " << std::setw( 10 ) << g_NLL << " | chi2 = " << std::setw( 10 ) << g_Chi2 << " | s0 = " << std::setw( 10 ) << pars[ Par::s0 ] << " | s1 = " << std::setw( 10 ) << pars[ Par::s1 ] << " | p0 = " << std::setw( 10 ) << pars[ Par::p0 ] << " | p1 = " << std::setw( 10 ) << pars[ Par::p1 ] << " | p2 = " << std::setw( 10 ) << pars[ Par::p2 ] ); 
  }
}

void MuonCalibrationFit::SaveParameters( TString name, const double* pars, const double* errs ) {
  SaveParameters( name, pars, errs, errs );
}

void MuonCalibrationFit::Stop( Tools::RegionInfo* fitRegion ) {
  g_DataSingleInfo->clear();
  g_DataBothInfo->clear();
  g_BackgroundSingleInfo->clear();
  g_BackgroundBothInfo->clear();
  g_MonteCarloSingleInfo->clear();
  g_MonteCarloBothInfo->clear();
  g_Tree->Write();
  //:::
  for( int index = 0; index < Par::N; index++ ) {
    fitRegion->par[ index ] = g_pars[ index ];
    fitRegion->err_up[ index ] = g_errs_up[ index ];
    fitRegion->err_low[ index ] = g_errs_low[ index ];
  }
  fitRegion->Corrected = kTRUE;
  fitRegion->SaveToFile( g_Output, g_Chi2, g_NLL );
  //:::
  delete g_CurrentTemplate;
}

void MuonCalibrationFit::GetMemoryUsage() {
  float vm_usage     = 0.;
  float resident_set = 0.;
  std::ifstream stat_stream( "/proc/self/stat", std::ios_base::in );
  std::string pid, comm, state, ppid, pgrp, session, tty_nr;
  std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  std::string utime, stime, cutime, cstime, priority, nice;
  std::string O, itrealvalue, starttime;
  unsigned long vsize;
  long rss;
  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
  >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
  >> utime >> stime >> cutime >> cstime >> priority >> nice
  >> O >> itrealvalue >> starttime >> vsize >> rss; 
  stat_stream.close();
  long page_size_kb = sysconf( _SC_PAGE_SIZE ) / 1024; 
  vm_usage     = vsize / 1024.0;
  resident_set = rss * page_size_kb;
  ATH_MSG_INFO( "Memory usage, VM: " << vm_usage << ", RSS: " << resident_set );
  return;
}

bool MuonCalibrationFit::RunMinimizationStep( std::string what, int times ) {
  ATH_MSG_INFO( "Minimization Step: Algorithm = " << what << ", Retrials = " << times );
  if( what == "Single" ) {
    return kTRUE;
  }
  else if( what == "Simplex" ) {
    ROOT::Math::Functor Fun( this, ( m_DoChi2 ) ? &MuonCalibrationFit::GetChiSquare : &MuonCalibrationFit::GetNegLogLikelihood, Par::N ); 
    ROOT::Math::Minimizer* MinSimplex = ROOT::Math::Factory::CreateMinimizer( "Minuit2", "Simplex" );
    MinSimplex->SetPrintLevel( 0 );
    MinSimplex->SetStrategy( 2 );
    MinSimplex->SetTolerance( 1 );
    MinSimplex->SetMaxFunctionCalls( 100000 );
    MinSimplex->SetFunction( Fun );
    MinSimplex->SetErrorDef( m_Error );
    bool successful_fit = kFALSE;
    int fit_trial = 0;
    TRandom random_value( 0 );
    while( ! successful_fit && fit_trial < times ) {
      if( g_FitTrial > 0 ) {
        for( int index = 0; index < Par::N; index++ ) {
          m_Init[ index ] = random_value.Uniform( m_MinInit[ index ], m_MaxInit[ index ] );
          m_Step[ index ] = random_value.Uniform( m_MinStep[ index ], m_MaxStep[ index ] );
        }
      }
      SetVariables( MinSimplex );
      ATH_MSG_INFO( "fit_trial = " << fit_trial << ", ndof: " << g_CurrentTemplate->GetDegreesOfFreedom() );
      successful_fit = ( MinSimplex->Minimize() && TMath::Prob( MinSimplex->MinValue(), g_CurrentTemplate->GetDegreesOfFreedom() ) > m_ProbThreshold );
      fit_trial++;
      g_FitTrial++;
    }
    const double *fit_pars = MinSimplex->X();
    const double *fit_errors = MinSimplex->Errors();
    double pars[ Par::N ];
    double errors[ Par::N ];
    FoldVariables( fit_pars, fit_errors, pars, errors );
    ATH_MSG_INFO( "Simplex minimum found (no errors given):" );
    for( int index = 0; index < Par::N; index++ ) {
      ATH_MSG_INFO( "  " << m_ParNames[ index ] << " = " << std::setw( 15 ) << std::left << pars[ index ] );
    }
    if( m_DoNLL )  ATH_MSG_INFO( " nll = " << std::setw( 15 ) << std::left << MinSimplex->MinValue() ); 
    if( m_DoChi2 ) ATH_MSG_INFO( "chi2 = " << std::setw( 15 ) << std::left << MinSimplex->MinValue() ); 
    UpdateGlobalValues( MinSimplex->MinValue(), pars, errors );
    DoMonitoringPlots( g_pars );
    g_CurrentTemplate->Write( "PostSimplexFit" ); 
    SaveParameters( "PostSimplexFit", pars, errors );
    for( int index = 0; index < Par::N; index++ ) {
      m_Init[ index ] = pars[ index ];
    }
    return kTRUE;
  }
  else if( what == "Migrad" ) {
    ROOT::Math::Functor Fun( this, ( m_DoChi2 ) ? &MuonCalibrationFit::GetChiSquare : &MuonCalibrationFit::GetNegLogLikelihood, Par::N ); 
    ROOT::Math::Minimizer* MinMigrad = ROOT::Math::Factory::CreateMinimizer( "Minuit2", "Migrad" );
    MinMigrad->SetPrintLevel( 0 );
    MinMigrad->SetStrategy( 2 );
    MinMigrad->SetTolerance( 1 );
    MinMigrad->SetPrecision( 0.001 );
    MinMigrad->SetMaxFunctionCalls( 100000 );
    MinMigrad->SetFunction( Fun );
    MinMigrad->SetErrorDef( m_Error );
    bool successful_fit = kFALSE;
    int fit_trial = 0;
    TRandom random_value( 0 );
    while( ! successful_fit && fit_trial < times ) {
      if( fit_trial > 0 ) {
        for( int index = 0; index < Par::N; index++ ) {
          m_Init[ index ] = random_value.Uniform( m_MinInit[ index ], m_MaxInit[ index ] );
          m_Step[ index ] = random_value.Uniform( m_MinStep[ index ], m_MaxStep[ index ] );
        }
      }
      SetVariables( MinMigrad );
      successful_fit = MinMigrad->Minimize(); 
      fit_trial++;
    }
    const double *fit_pars = MinMigrad->X();
    const double *fit_errors = MinMigrad->Errors();
    double pars[ Par::N ];
    double errors[ Par::N ];
    FoldVariables( fit_pars, fit_errors, pars, errors );
    ATH_MSG_INFO( "Migrad minimum found:" );
    for( int index = 0; index < Par::N; index++ ) {
      ATH_MSG_INFO( "  " << m_ParNames[ index ] << " = " << std::setw( 15 ) << std::left << pars[ index ] << " +/- " << std::setw( 15 ) << std::left << errors[ index ] );
    }
    if( m_DoNLL )  ATH_MSG_INFO( " nll = " << std::setw( 15 ) << std::left << MinMigrad->MinValue() ); 
    if( m_DoChi2 ) ATH_MSG_INFO( "chi2 = " << std::setw( 15 ) << std::left << MinMigrad->MinValue() ); 
    UpdateGlobalValues( MinMigrad->MinValue(), pars, errors );
    DoMonitoringPlots( g_pars );
    g_CurrentTemplate->Write( "PostMigradFit" ); 
    SaveParameters( "PostMigradFit", pars, errors );
    for( int index = 0; index < Par::N; index++ ) {
      m_Init[ index ] = pars[ index ];
    }
    return kTRUE;
  }
  else if( what == "SimpleScan" ) { 
    double pars[ Par::N ];
    for( int index = 0; index < Par::N; index++ ) {
      pars[ index ] = m_Init[ index ];
    }
    //::://::://::: Run scans "times" times
    int AddIterCounter = 0;
    while( g_ScanIter > 0 && AddIterCounter++ < times ) {
      ATH_MSG_INFO( "Running Scans, Iteration n." << g_ScanIter );
      //::://::://::: Calculating by hand optimal range for scan
      g_ScanPar = -1;
      double final_alpha_pos[ Par::N ] = { 0., 0., 0., 0., 0. };
      double final_alpha_neg[ Par::N ] = { 0., 0., 0., 0., 0. };
      TRandom random_value( 0 );
      for( int var_index = 0; var_index < Par::N; var_index++ ) {
        if( ! m_Use[ var_index ] ) continue; // It means the variable is turned off in the fit, no need to scan it!
        double alpha_pos[ Par::N ] = { 0., 0., 0., 0., 0. };
        alpha_pos[ var_index ] = 0.5;
        bool found = kFALSE;
        while( ! found ) {
          ATH_MSG_DEBUG( "alpha_pos = " << alpha_pos[ var_index ] << ", temp_pars[ var_index ] = " << pars[ var_index ] * ( 1. + alpha_pos[ var_index ] ) );
          double temp_pars[ Par::N ];
          for( int var2_index = 0; var2_index < Par::N; var2_index++ ) {
            if( TMath::AreEqualAbs( pars[ var2_index ], 0, 0.0000001 ) ) {
              temp_pars[ var2_index ] = alpha_pos[ var2_index ]; 
            }
            else {
              temp_pars[ var2_index ] = pars[ var2_index ] * ( 1. + alpha_pos[ var2_index ] );
            }
          }
          // for( int var2_index = 0; var2_index < Par::N; var2_index++ ) {
          //   ATH_MSG_DEBUG( "temp_pars[ " << var2_index << " ] = " << temp_pars[ var2_index ] );
          //   ATH_MSG_DEBUG( "pars[ " << var2_index << " ] = " << pars[ var2_index ] );
          //   ATH_MSG_DEBUG( "alpha_pos[ " << var2_index << " ] = " << alpha_pos[ var2_index ] );
          // }
          double TempMin = GetFunction( temp_pars );
          double Dist = fabs( TempMin - g_GlobalMinimum );
          //::: if between 2 and 3 sigmas on one side
          if( Dist > 4 * m_Error && Dist < 9 * m_Error ) {
            found = kTRUE;
            final_alpha_pos[ var_index ] = alpha_pos[ var_index ];
            ATH_MSG_DEBUG( "Eureka: alpha_pos[ " << var_index << " ] = " << alpha_pos[ var_index ] << "  ===>  pars[ var_index ] = " << pars[ var_index ] * ( 1. + alpha_pos[ var_index ] ) );
          }
          else {
            alpha_pos[ var_index ] *= sqrt( random_value.Uniform( 4 * m_Error, 9 * m_Error ) / Dist );
          }
        }
        double alpha_neg[ Par::N ] = { 0., 0., 0., 0., 0. };
        alpha_neg[ var_index ] = 0.1;
        found = kFALSE;
        while( ! found ) {
          ATH_MSG_DEBUG( "alpha_neg = " << alpha_neg[ var_index ] << ", temp_pars[ var_index ] = " << pars[ var_index ] * ( 1. - alpha_neg[ var_index ] ) );
          double temp_pars[ Par::N ];
          for( int var2_index = 0; var2_index < Par::N; var2_index++ ) {
            if( TMath::AreEqualAbs( pars[ var2_index ], 0, 0.0000001 ) ) {
              temp_pars[ var2_index ] = - alpha_neg[ var2_index ]; 
            }
            else {
              temp_pars[ var2_index ] = pars[ var2_index ] * ( 1. - alpha_neg[ var2_index ] );
            }
          }
          // for( int var2_index = 0; var2_index < Par::N; var2_index++ ) {
          //   ATH_MSG_DEBUG( "temp_pars[ " << var2_index << " ] = " << temp_pars[ var2_index ] );
          //   ATH_MSG_DEBUG( "pars[ " << var2_index << " ] = " << pars[ var2_index ] );
          //   ATH_MSG_DEBUG( "alpha_neg[ " << var2_index << " ] = " << alpha_neg[ var2_index ] );
          // }
          double TempMin = GetFunction( temp_pars );
          double Dist = fabs( TempMin - g_GlobalMinimum );
          //::: if between 2 and 3 sigmas on one side
          if( Dist > 4 * m_Error && Dist < 9 * m_Error ) {
            found = kTRUE;
            final_alpha_neg[ var_index ] = alpha_neg[ var_index ];
            ATH_MSG_DEBUG( "Eureka: alpha_neg[ " << var_index << " ] = " << alpha_neg[ var_index ] << "  ===>  pars[ var_index ] = " << pars[ var_index ] * ( 1. - alpha_neg[ var_index ] ) );
          }
          else {
            alpha_neg[ var_index ] *= sqrt( random_value.Uniform( 4 * m_Error, 9 * m_Error ) / Dist );
            if( m_PosDef[ var_index ] && pars[ var_index ] * ( 1. - alpha_neg[ var_index ] ) < 0 ) {
              ATH_MSG_DEBUG( "Ouch! alpha_neg = " << alpha_neg[ var_index ] << ", temp_pars[ var_index ] = " << pars[ var_index ] * ( 1. - alpha_neg[ var_index ] ) );
              found = kTRUE;
              alpha_neg[ var_index ] = 1.; 
              final_alpha_neg[ var_index ] = alpha_neg[ var_index ];
              ATH_MSG_DEBUG( "Eureka (forded): alpha_neg[ " << var_index << " ] = " << alpha_neg[ var_index ] << "  ===>  pars[ var_index ] = " << pars[ var_index ] * ( 1. - alpha_neg[ var_index ] ) );
            }
          }
        }
      }
      //::://::://::: Scanning all available variables
      double scanCenter[ Par::N ], scanSteps[ Par::N ], errors_up[ Par::N ], errors_low[ Par::N ];
      for( int the_index = 0; the_index < Par::N; the_index++ ) {
        scanCenter[ the_index ] = pars[ the_index ] * ( 2 + final_alpha_pos[ the_index ] - final_alpha_neg[ the_index ] ) / 2.;
        scanSteps[ the_index ] = fabs( pars[ the_index ] * ( final_alpha_pos[ the_index ] + final_alpha_neg[ the_index ] ) / 20. );
      }
      bool NewMinimumFound = kFALSE;
      for( int the_index = 0; the_index < Par::N; the_index++ ) {
        if( ! m_Use[ the_index ] ) {
          errors_up[ the_index ] = 0.;
          errors_low[ the_index ] = 0.;
          continue; // It means the variable is turned off in the fit, no need to scan it!
        }
        //::: Scanning specific variable
        ATH_MSG_INFO( "Scanning variable n." << the_index );
        g_ScanPar = the_index;
        double previousX( -999 ), previousY( g_GlobalMinimum + 100 ), error_left( -999. ), error_right( -999. );
        double temp_pars[ Par::N ];
        for( int the_2nd_index = 0; the_2nd_index < Par::N; the_2nd_index++ ) {
          temp_pars[ the_2nd_index ] = pars[ the_2nd_index ];
        }
        //::: Loop on steps
        for( auto index: boost::irange( - 10, + 10 ) ) {
          temp_pars[ the_index ] = scanCenter[ the_index ] + scanSteps[ the_index ] * index;
          double currentX = temp_pars[ the_index ]; 
          double currentY = GetFunction( temp_pars );
          //::: Case 1: current sampling sufficiently < than global minimum
          NewMinimumFound = ( currentY < g_GlobalMinimum && ! TMath::AreEqualAbs( currentY, g_GlobalMinimum, 0.1 ) ); 
          // if( currentY < g_GlobalMinimum && ! TMath::AreEqualAbs( currentY, g_GlobalMinimum, 0.1 ) && AddIterCounter != times ) {
          //   g_GlobalMinimum = currentY;
          //   g_ScanIter++;
          //   for( int the_2nd_index = 0; the_2nd_index < Par::N; the_2nd_index++ ) {
          //     pars[ the_2nd_index ] = temp_pars[ the_2nd_index ];
          //   }
          //   error_left = -999.;
          //   error_right = -999.;
          //   break; 
          // }
          //::: Case 2: previous sampling > m_Error and current < m_Error
          if( previousY - g_GlobalMinimum > m_Error && currentY - g_GlobalMinimum < m_Error ) {
            // ATH_MSG_DEBUG( "----- Case 2 -----" );
            // ATH_MSG_DEBUG( "-----       Previous: ( " << previousX << ", " << previousY << " )" );
            // ATH_MSG_DEBUG( "-----        Current: ( " << currentX << ", " << currentY << " )" );
            // ATH_MSG_DEBUG( "----- Global Minimum: ( " << pars[ the_index ] << ", " << g_GlobalMinimum << " )" );
            double interY = g_GlobalMinimum + m_Error;
            double interX = ( currentX * ( previousY - interY ) + previousX * ( interY - currentY ) ) / ( previousY - currentY );
            error_left = fabs( interX - pars[ the_index ] );
            // ATH_MSG_DEBUG( "-----   Interpolated: ( " << interX << ", " << interY << " )" );
            // ATH_MSG_DEBUG( "-----          Error: ( " << error_left << " )" );
          }
          //::: Case 3: previous sampling < m_Error and current > m_Error
          if( previousY - g_GlobalMinimum < m_Error && currentY - g_GlobalMinimum > m_Error ) {
            // ATH_MSG_DEBUG( "----- Case 3 -----" );
            // ATH_MSG_DEBUG( "-----       Previous: ( " << previousX << ", " << previousY << " )" );
            // ATH_MSG_DEBUG( "-----        Current: ( " << currentX << ", " << currentY << " )" );
            // ATH_MSG_DEBUG( "----- Global Minimum: ( " << pars[ the_index ] << ", " << g_GlobalMinimum << " )" );
            double interY = g_GlobalMinimum + m_Error;
            double interX = ( currentX * ( previousY - interY ) + previousX * ( interY - currentY ) ) / ( previousY - currentY );
            error_right = fabs( interX - pars[ the_index ] );
            // ATH_MSG_DEBUG( "-----   Interpolated: ( " << interX << ", " << interY << " )" );
            // ATH_MSG_DEBUG( "-----          Error: ( " << error_right << " )" );
          }
          previousX = currentX;
          previousY = currentY;
        }
        //::: Check if reasonable errors found
        if( error_left > 0. && error_right > 0. ) {
          errors_up[ the_index ] = error_right;
          errors_low[ the_index ] = error_left;
        }
        else {
          //::: If reasonable error not found, it is time to start again!
          errors_up[ the_index ] = -999.;
          errors_low[ the_index ] = -999.;
          break;
        }
      }
      bool AllScansDone = kTRUE;
      for( int the_index = 0; the_index < Par::N; the_index++ ) {
        if( ! m_Use[ the_index ] ) continue;
        if( errors_up[ the_index ] < 0 ) AllScansDone = kFALSE;
      }
      if( AllScansDone || ( AddIterCounter == times ) ) {
        //::: Time to exit :D
        if( ! NewMinimumFound && AllScansDone ) g_ScanIter = -1; //::: We will not need additional scans!
        ATH_MSG_INFO( "Scan minimum found:" );
        for( int index = 0; index < Par::N; index++ ) {
          ATH_MSG_INFO( "  " << m_ParNames[ index ] << " = " << std::setw( 15 ) << std::left << pars[ index ] << " + " << std::setw( 15 ) << std::left << errors_up[ index ] << " - " << std::setw( 15 ) << std::left << errors_low[ index ] );
        }
        if( m_DoNLL )  ATH_MSG_INFO( " nll = " << std::setw( 15 ) << std::left << g_GlobalMinimum ); 
        if( m_DoChi2 ) ATH_MSG_INFO( "chi2 = " << std::setw( 15 ) << std::left << g_GlobalMinimum ); 
        UpdateGlobalValues( g_GlobalMinimum, pars, errors_up, errors_low );
        DoMonitoringPlots( g_pars );
        g_CurrentTemplate->Write( "PostScanFit" ); 
        SaveParameters( "PostScanFit", pars, errors_up, errors_low ); 
        //::: Reset Initial Parameters
        // for( int index = 0; index < Par::N; index++ ) {
        //   m_Init[ index ] = 0.; 
        // }
        //::: Pass fitted parameters to next step only if new minimum has not been found!
        // if( ! NewMinimumFound && AllScansDone ) {
        //   for( int index = 0; index < Par::N; index++ ) {
        //     m_Init[ index ] = pars[ index ];
        //   }
        // }
      }
      else { 
        continue;
      }
    }
    if( g_ScanIter > 0 ) {
      g_ScanIter++;
      ATH_MSG_INFO( "Scanning procedure failed, restarting from top of chain..." );
      return kFALSE;
    }
    else {
      return kTRUE;
    }
  }
  return kTRUE;
}

void MuonCalibrationFit::UpdateTemplates( const double* par ) {
  g_CurrentTemplate->ClearMonteCarlo(); 
  for( int index = 0; index < Par::N; index++ ) {
    g_pars[ index ] = par[ index ];
  }
  //::: Uncorrected Types
  if( ! g_FilledData ) {
    //::: Data
    g_CurrentTemplate->ClearData();
    BOOST_FOREACH( Tools::Info theInfo, *g_DataSingleInfo ) {
      if( ! theInfo.IsOS ) continue;
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), theInfo.First_Mu_pt );
      //::: Method 2: pt bin chosen with leading muon!
      g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ) );
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::Data, theInfo.GetRho().first, theInfo.First_Mu_pt );
      g_CurrentTemplate->FillRho( Type::Data, theInfo.GetRho().second, theInfo.Second_Mu_pt );
    }
    BOOST_FOREACH( Tools::Info theInfo, *g_DataBothInfo ) {
      if( ! theInfo.IsOS ) continue;
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), theInfo.First_Mu_pt );
      //g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), theInfo.Second_Mu_pt );
      //::: Method 2: pt bin chosen with leading muon!
      //theInfo.Print( Type::Data );
      g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ) );
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::Data, theInfo.GetRho().first, theInfo.First_Mu_pt );
      g_CurrentTemplate->FillRho( Type::Data, theInfo.GetRho().second, theInfo.Second_Mu_pt );
    }
    //::: Background
    g_CurrentTemplate->ClearBackground();
    BOOST_FOREACH( Tools::Info theInfo, *g_BackgroundSingleInfo ) {
      if( ! theInfo.IsOS ) continue;
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), theInfo.First_Mu_pt, theInfo.Weight );
      //::: Method 2: pt bin chosen with leading muon!
      g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ), theInfo.Weight );
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::Background, theInfo.GetRho().first, theInfo.First_Mu_pt, theInfo.Weight );
      g_CurrentTemplate->FillRho( Type::Background, theInfo.GetRho().second, theInfo.Second_Mu_pt, theInfo.Weight );
    }
    BOOST_FOREACH( Tools::Info theInfo, *g_BackgroundBothInfo ) {
      if( ! theInfo.IsOS ) continue;
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), theInfo.First_Mu_pt, theInfo.Weight );
      //g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), theInfo.Second_Mu_pt, theInfo.Weight );
      //::: Method 2: pt bin chosen with leading muon!
      g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ), theInfo.Weight );
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::Background, theInfo.GetRho().first, theInfo.First_Mu_pt, theInfo.Weight );
      g_CurrentTemplate->FillRho( Type::Background, theInfo.GetRho().second, theInfo.Second_Mu_pt, theInfo.Weight );
    }
    //::://::://:::
    g_FilledData = kTRUE;
  }
  //::: MonteCarlo
  //::: Single Muon Correction
  BOOST_FOREACH( Tools::Info theInfo, *g_MonteCarloSingleInfo ) {
    if( ! theInfo.IsOS ) continue;
    TRandom2 rand_gen( int( TMath::Abs( theInfo.Gamma ) * 1000000 ) );
    for( int index = 0; index < m_NumRandomValues; index++ ) {
      double rnd = rand_gen.Gaus( 0, 1 );
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassSingle( par, rnd ), theInfo.Corr_First_Mu_pt( par, rnd ), theInfo.Weight / ( float ) m_NumRandomValues );
      //::: Method 2: pt bin chosen with leading muon!
      g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassSingle( par, rnd ), std::max( theInfo.Corr_First_Mu_pt( par, rnd ), theInfo.Second_Mu_pt ), theInfo.Weight / ( float ) m_NumRandomValues );
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::MC, theInfo.GetRho( par, rnd ).first, theInfo.Corr_First_Mu_pt( par, rnd ), theInfo.Weight / ( float ) m_NumRandomValues );
      g_CurrentTemplate->FillRho( Type::MC, theInfo.GetRho( par, rnd ).second, theInfo.Corr_Second_Mu_pt( par, rnd ), theInfo.Weight / ( float ) m_NumRandomValues );
    }
  }
  //::: Double Muon Correction
  BOOST_FOREACH( Tools::Info theInfo, *g_MonteCarloBothInfo ) {
    if( ! theInfo.IsOS ) continue;
    TRandom2 rand_gen( int( TMath::Abs( theInfo.Gamma ) * 1000000 ) );
    for( int index = 0; index < m_NumRandomValues; index++ ) {
      double rnd1 = rand_gen.Gaus( 0, 1 );
      double rnd2 = rand_gen.Gaus( 0, 1 );
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassBoth( par, rnd1, rnd2 ), theInfo.Corr_First_Mu_pt( par, rnd1 ), theInfo.Weight / ( float ) m_NumRandomValues );
      //g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassBoth( par, rnd1, rnd2 ), theInfo.Corr_Second_Mu_pt( par, rnd2 ), theInfo.Weight / ( float ) m_NumRandomValues );
      //::: Method 2: pt bin chosen with leading muon!
      g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassBoth( par, rnd1, rnd2 ), std::max( theInfo.Corr_First_Mu_pt( par, rnd1 ), theInfo.Corr_Second_Mu_pt( par, rnd2 ) ), theInfo.Weight / ( float ) m_NumRandomValues );
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::MC, theInfo.GetRho( par, rnd1 ).first, theInfo.Corr_First_Mu_pt( par, rnd1 ), theInfo.Weight / ( float ) m_NumRandomValues );
      g_CurrentTemplate->FillRho( Type::MC, theInfo.GetRho( par, rnd2 ).second, theInfo.Corr_Second_Mu_pt( par, rnd2 ), theInfo.Weight / ( float ) m_NumRandomValues );
    }
  }
  g_UpdatedTemplates++;
}

void MuonCalibrationFit::FoldVariables( const double* fit_pars, const double* fit_errs, double* pars, double* errs ) {
  for( int index = 0; index < Par::N; index++ ) {
    if( m_PosDef[ index ] ) {
      pars[ index ] = TMath::Abs( fit_pars[ index ] );
    }
    else { 
      pars[ index ] = fit_pars[ index ];
    }
    errs[ index ] = fit_errs[ index ];
  }
}

void MuonCalibrationFit::UpdateGlobalValues( double min, const double* pars, const double* errs_up, const double* errs_low ) {
  g_GlobalMinimum = min;
  for( int index = 0; index < Par::N; index++ ) {
    if( m_PosDef[ index ] && pars[ index ] < 0 ) {
      g_pars[ index ] = - pars[ index ];
      g_errs_up[ index ] = errs_low[ index ];
      g_errs_low[ index ] = errs_up[ index ];
    }
    else {
      g_pars[ index ] = pars[ index ];
      g_errs_up[ index ] = errs_up[ index ];
      g_errs_low[ index ] = errs_low[ index ];
    }
  }
  GetFunction( pars );
}

void MuonCalibrationFit::UpdateGlobalValues( double min, const double* pars, const double* errs ) {
  UpdateGlobalValues( min, pars, errs, errs ); 
}

void MuonCalibrationFit::SetVariables( ROOT::Math::Minimizer* Minim ) {
  for( int index = 0; index < Par::N; index++ ) {
    if( ! m_Use[ index ] ) {
      ATH_MSG_INFO( "Setting Constant Variable Named: " << m_ParNames[ index ] );
      Minim->SetFixedVariable( index, m_ParNames[ index ], 0 ); 
    }
    //::: Deactivated due to possible bug: step not used!!!!!!!
    //::: else if( m_PosDef[ index ] ) {
    //:::   ATH_MSG_INFO( "Setting Lower Limited Variable Named: " << m_ParNames[ index ] << ", Init: " << m_Init[ index ] << ", Step: " << m_Step[ index ] );
    //:::   Minim->SetLowerLimitedVariable( index, m_ParNames[ index ], m_Init[ index ], m_Step[ index ], 0. );
    //::: }
    else {
      ATH_MSG_INFO( "Setting Variable Named: " << m_ParNames[ index ] << ", Init: " << m_Init[ index ] << ", Step: " << m_Step[ index ] );
      Minim->SetVariable( index, m_ParNames[ index ], m_Init[ index ], m_Step[ index ] );
    }
  }
}

float MuonCalibrationFit::GetTruth( float eta, int par ) {
  if( par == Par::s0 ) {
    TF1 func( "func", "0.", -2.5, 2.5 ); 
    return func.Eval( eta );
  }
  if( par == Par::s1 ) {
    TF1 func( "func", "0.01 * TMath::Sin( 2 * x ) / ( x + 5 )", -2.5, 2.5 );
    return func.Eval( eta );
    //return -0.001;
  }
  if( par == Par::p0 ) {
    TF1 func( "func", "0.", -2.5, 2.5 ); 
    return func.Eval( eta );
  }
  if( par == Par::p1 ) {
    TF1 func( "func", "0.002 + 0.01 * TMath::Exp( - TMath::Power( x * 0.5, 2 ) )", -2.5, 2.5 );
    return func.Eval( eta );
    //return 0.01;
  }
  if( par == Par::p2 ) {
    TF1 func( "func", "0.00016 * ( 2 + TMath::Erf( x ) )", -2.5, 2.5 );
    return func.Eval( eta );
    //return 0.0005;
  }
  else {
    return -666.;
  }
}
