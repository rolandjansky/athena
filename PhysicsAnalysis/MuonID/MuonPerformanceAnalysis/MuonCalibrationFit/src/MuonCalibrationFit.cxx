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
  declareProperty( "JpsiReweightingFile", m_JpsiReweightingFile = "JpsiReweighting.root" );
  declareProperty( "ZReweightingFile", m_ZReweightingFile = "ZReweighting.root" );
  declareProperty( "MaxEvents", m_MaxEvents = -1 );
  declareProperty( "MonitoringPlots", m_MonitoringPlots = kFALSE );
  declareProperty( "RequireErrors", m_RequireErrors = kFALSE );
  declareProperty( "GRL_Name", m_GRL_Name = "" );
  //::: Fitting properties
  declareProperty( "Detector", m_Detector = "ID" );
  declareProperty( "FitType", m_FitType = "Chi2" );
  declareProperty( "BkgParametrization", m_Bkg = "Exponential" );
  declareProperty( "RunMode", m_RunMode = "Simplex:1" );
  declareProperty( "MaxRetrials", m_MaxRetrials = 3 );
  declareProperty( "ProbThreshold", m_ProbThreshold = 0.1 );
  //::: Parameters' properties
  declareProperty( "S0_Init", m_Init[ Par::s0 ] = 0. ); //::: ???
  declareProperty( "S0_Step", m_Step[ Par::s0 ] = 0.1 );
  declareProperty( "S1_Init", m_Init[ Par::s1 ] = 0. ); //::: -0.001
  declareProperty( "S1_Step", m_Step[ Par::s1 ] = 0.001 );
  declareProperty( "P0_Init", m_Init[ Par::p0 ] = 0. ); //::: ???
  declareProperty( "P0_Step", m_Step[ Par::p0 ] = 0.01 );
  declareProperty( "P1_Init", m_Init[ Par::p1 ] = 0. ); //::: 0.01
  declareProperty( "P1_Step", m_Step[ Par::p1 ] = 0.001 );
  declareProperty( "P2_Init", m_Init[ Par::p2 ] = 0. ); //::: 0.0005
  declareProperty( "P2_Step", m_Step[ Par::p2 ] = 0.0001 );
  declareProperty( "Use_S0", m_Use[ Par::s0 ] = kFALSE );
  declareProperty( "Use_S1", m_Use[ Par::s1 ] = kFALSE );
  declareProperty( "Use_P0", m_Use[ Par::p0 ] = kFALSE );
  declareProperty( "Use_P1", m_Use[ Par::p1 ] = kFALSE );
  declareProperty( "Use_P2", m_Use[ Par::p2 ] = kFALSE );
  //::: Templates' properties
  declareProperty( "Splitting", m_Splitting = 1 );
  declareProperty( "NumRandomValues", m_NumRandomValues = 1 );
  declareProperty( "MinMuonsPtForJpsi", m_MinMuonsPtForJpsi = 8 );
  declareProperty( "MaxMuonsPtForJpsi", m_MaxMuonsPtForJpsi = 17 );
  declareProperty( "MinLeadingMuonPtForZ", m_MinLeadingMuonPtForZ = 26 );
  declareProperty( "MinSubLeadingMuonPtForZ", m_MinSubLeadingMuonPtForZ = 15 );
  declareProperty( "JpsiMass_Bins", m_JpsiMass_Bins = 80 );
  declareProperty( "JpsiMass_Min", m_JpsiMass_Min = 2.7 );
  declareProperty( "JpsiMass_Max", m_JpsiMass_Max = 3.5 );
  declareProperty( "JpsiMass_Pts", m_JpsiMass_Pts = "" );
  declareProperty( "ZMass_Bins", m_ZMass_Bins = 50 );
  declareProperty( "ZMass_Min", m_ZMass_Min = 66 );
  declareProperty( "ZMass_Max", m_ZMass_Max = 116 );
  declareProperty( "ZMass_Pts", m_ZMass_Pts = "" );
  declareProperty( "ZRho_Bins", m_ZRho_Bins = 50 );
  declareProperty( "ZRho_Min", m_ZRho_Min = -0.3 );
  declareProperty( "ZRho_Max", m_ZRho_Max = 0.3 );
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
  ATH_MSG_DEBUG( "GRL_Name:" << m_GRL_Name );
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
  ATH_MSG_DEBUG( "ZRho_Bins: " << m_ZRho_Bins );
  ATH_MSG_DEBUG( "ZRho_Min: " << m_ZRho_Min );
  ATH_MSG_DEBUG( "ZRho_Max: " << m_ZRho_Max );
  ATH_MSG_DEBUG( "ZRho_Pts: " << m_ZRho_Pts );
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
  m_MinStep[ Par::p1 ] = 0.001;   m_MaxStep[ Par::p1 ] = 0.01;    
  m_MinStep[ Par::p2 ] = 0.00001; m_MaxStep[ Par::p2 ] = 0.001;    

  m_Prev[ Par::s0 ] = m_Init[ Par::s0 ];
  m_Prev[ Par::s1 ] = m_Init[ Par::s1 ];
  m_Prev[ Par::p0 ] = m_Init[ Par::p0 ];
  m_Prev[ Par::p1 ] = m_Init[ Par::p1 ];
  m_Prev[ Par::p2 ] = m_Init[ Par::p2 ];
  //:::
  //if( m_Detector == "ID" ) {
  //  //::: Some overrides, just to be 110% sure...
  //  m_Use[ Par::s0 ] = kFALSE;
  //  m_Use[ Par::p0 ] = kFALSE;
  //}
  //:::
  return StatusCode::SUCCESS;
}

StatusCode MuonCalibrationFit::execute() {
  ATH_MSG_INFO( "executing MuonCalibrationFit" );
  Regions* InReg = new Regions( m_InputRegionsFile, m_Input, kFALSE );
  InReg->Print();
  Regions* InRegForID = NULL;
  if( m_InputRegionsFileForID != "" && m_InputForID != "" ) {
    InRegForID = new Regions( m_InputRegionsFileForID, m_InputForID, kFALSE );
    //InRegForID->Print();
  }
  Regions* OutReg = new Regions( m_OutputRegionsFile, "", kFALSE );
  //OutReg->Print();
  //::: Loop on Regions
  std::vector< Tools::RegionInfo* > InRegInfos = InReg->GetRegions();
        
  for( std::vector< Tools::RegionInfo* >::iterator It = InRegInfos.begin(); It != InRegInfos.end(); ++It ) ATH_MSG_DEBUG( ( *It )->GetInfo() ); 
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
    g_best_NLL = 1.E09;
    g_best_Chi2 = 1.E09;
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
    //::://::://::: NoCorrections part 
    FillVectors( InRegInfos, InRegInfosForID, *It, false );
    const double nocorr_pars[ Par::N ] = { 0, 0, 0, 0, 0 };
    const double nocorr_errors[ Par::N ] = { 0, 0, 0, 0, 0 };
    GetFunction( nocorr_pars );
    if( m_MonitoringPlots ) DoMonitoringPlots( nocorr_pars );
    g_CurrentTemplate->Write( "NoCorrections" ); 
    SaveParameters( "NoCorrections", nocorr_pars, nocorr_errors );
    //::://::://::: InitialCorrections part
    FillVectors( InRegInfos, InRegInfosForID, *It, true );
    const double init_pars[ Par::N ] = { m_Init[ Par::s0 ], m_Init[ Par::s1 ], m_Init[ Par::p0 ], m_Init[ Par::p1 ], m_Init[ Par::p2 ] };
    const double init_errors[ Par::N ] = { 0, 0, 0, 0, 0 };
    if( m_RunMode == "" ) {
      GetFunction( nocorr_pars );
    }
    else {
      GetFunction( init_pars );
    }
    if( m_MonitoringPlots ) DoMonitoringPlots( init_pars );
    g_CurrentTemplate->Write( "InitialCorrections" ); 
    SaveParameters( "InitialCorrections", init_pars, init_errors );
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
        size_t init = algo_name.find( "[" ) + 1;
        size_t end = algo_name.find( "]" );
        std::string active_pars = "";
        if( ( init != std::string::npos ) && ( end != std::string::npos ) ) {
          active_pars = algo_name.substr( init, end - init );
        }
        //::: Reset the variables to be used according to the general job configuration
        ResetParameters();
        ActivateParameters( active_pars );
        converged = RunMinimizationStep( algo_name.substr( 0, init - 1 ), max_retrials );
        ATH_MSG_DEBUG( "converged " << converged );
        CopyToBestFitParameters();
      }
    }
    //::://::://:::
    Stop( *It );
    ATH_MSG_DEBUG( ( *It )->GetInfo() );
  }
  g_Output->Close();
  delete InReg;
  delete OutReg;
  if( InRegForID ) { delete InRegForID; InRegForID = NULL; }
  return StatusCode::SUCCESS;
}

void MuonCalibrationFit::ActivateParameters( std::string active_pars ) {
  ATH_MSG_DEBUG( "active_pars ---->" << active_pars << "<----" );
  //:::
  if( active_pars == "" ) {
    for( int index_par = 0; index_par < Par::N; index_par++ ) {
      m_UseLocally[ index_par ] = m_Use[ index_par ]; 
    }
    return;
  }
  //::: First, turn them all off!
  for( int index_par = 0; index_par < Par::N; index_par++ ) m_UseLocally[ index_par ] = kFALSE; 
  //:::
  boost::char_separator< char > sep( "," );
  boost::tokenizer< boost::char_separator< char > > tokens( active_pars, sep );
  bool problem_parsing = false;
  for( boost::tokenizer< boost::char_separator< char > >::iterator token = tokens.begin(); token != tokens.end(); ++token ) {
    std::string par_name = *token; 
    if( par_name == "s0" ) m_UseLocally[ Par::s0 ] = kTRUE; 
    else if( par_name == "s1" ) m_UseLocally[ Par::s1 ] = kTRUE; 
    else if( par_name == "p0" ) m_UseLocally[ Par::p0 ] = kTRUE; 
    else if( par_name == "p1" ) m_UseLocally[ Par::p1 ] = kTRUE; 
    else if( par_name == "p2" ) m_UseLocally[ Par::p2 ] = kTRUE; 
    else problem_parsing = true;
  }
  if( problem_parsing ) {
    ATH_MSG_WARNING( "Unable to properly parse string (" << active_pars << "). For security reasons, set usage of all parameters to general configuration!" );
    for( int index_par = 0; index_par < Par::N; index_par++ ) m_UseLocally[ index_par ] = m_Use[ index_par ]; 
  }
  for( int index_par = 0; index_par < Par::N; index_par++ ) {
    ATH_MSG_DEBUG( "Parameter n." << index_par << ", general conf: " << m_Use[ index_par ] << ", local conf: " << m_UseLocally[ index_par ] ); 
  }
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
  TRandom3 rand_gen( 2988100123 );
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

void MuonCalibrationFit::FillVectors( std::vector< Tools::RegionInfo* > RegInfos, std::vector< Tools::RegionInfo* > RegInfosForID, Tools::RegionInfo* RegInfo, bool can_correct_all ) {

  //::: Debugging
  for( auto reg: RegInfos ) {
    ATH_MSG_INFO( reg->GetInfo() );
  }
  //::: General Flag: correct all muons with those obtained from previous iteration (for check job)
  bool correct_all = ( m_RunMode == "" && m_InputRegionsFile != "" && can_correct_all );
  //::: Data 
  g_DataSingleInfo->clear();
  g_DataBothInfo->clear();
  TChain chainData( "MuonMomentumCalibrationTree" );
  chainData.Add( m_DataFile.c_str() ); 
  Long_t MaxEvents = ( m_MaxEvents < 0 ) ? chainData.GetEntries() : m_MaxEvents;
  Reader readerData( Type::Data, m_GRL_Name, MaxEvents, ( m_Detector == "ID" ), !m_BatchMode, m_MinMuonsPtForJpsi, m_MaxMuonsPtForJpsi, m_MinLeadingMuonPtForZ, m_MinSubLeadingMuonPtForZ, RegInfos, RegInfosForID, RegInfo, g_DataSingleInfo, g_DataBothInfo, "", "", correct_all );
  chainData.Process( &readerData, "", MaxEvents, 0 );
  //::: Background 
  g_BackgroundSingleInfo->clear();
  g_BackgroundBothInfo->clear();
  TChain chainBackground( "MuonMomentumCalibrationTree" );
  chainBackground.Add( m_BackgroundFile.c_str() ); 
  MaxEvents = ( m_MaxEvents < 0 ) ? chainBackground.GetEntries() : m_MaxEvents;
  Reader readerBackground( Type::Background, "", MaxEvents, ( m_Detector == "ID" ), !m_BatchMode, m_MinMuonsPtForJpsi, m_MaxMuonsPtForJpsi, m_MinLeadingMuonPtForZ, m_MinSubLeadingMuonPtForZ, RegInfos, RegInfosForID, RegInfo, g_BackgroundSingleInfo, g_BackgroundBothInfo, "", "", correct_all );
  chainBackground.Process( &readerBackground, "", MaxEvents, 0 );
  //::: MonteCarlo 
  g_MonteCarloSingleInfo->clear();
  g_MonteCarloBothInfo->clear();
  TChain chainMonteCarlo( "MuonMomentumCalibrationTree" );
  chainMonteCarlo.Add( m_MonteCarloFile.c_str() ); 
  MaxEvents = ( m_MaxEvents < 0 ) ? chainMonteCarlo.GetEntries() : m_MaxEvents;
  Reader readerMonteCarlo( Type::MC, "", MaxEvents, ( m_Detector == "ID" ), !m_BatchMode, m_MinMuonsPtForJpsi, m_MaxMuonsPtForJpsi, m_MinLeadingMuonPtForZ, m_MinSubLeadingMuonPtForZ, RegInfos, RegInfosForID, RegInfo, g_MonteCarloSingleInfo, g_MonteCarloBothInfo, m_JpsiReweightingFile, m_ZReweightingFile, correct_all );
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
  TVectorD temp_vector( 3 * Par::N + 3 ); // For each parameter save value, error+, error-, likelihood and chi2
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
  temp_vector[ 3 * Par::N + 2 ] = g_CurrentTemplate->GetDegreesOfFreedom();
  TString template_name = g_CurrentTemplate->GetName();
  temp_vector.Write( name + "___" + template_name );
  if( name == "PreFit" || name == "FirstEstimate" || name == "TrueFit" ) {
    ATH_MSG_INFO( "  " << name << "  ---> nll = " << std::setw( 10 ) << g_NLL << " | chi2 = " << std::setw( 10 ) << g_Chi2 << " | s0 = " << std::setw( 10 ) << pars[ Par::s0 ] << " | s1 = " << std::setw( 10 ) << pars[ Par::s1 ] << " | p0 = " << std::setw( 10 ) << pars[ Par::p0 ] << " | p1 = " << std::setw( 10 ) << pars[ Par::p1 ] << " | p2 = " << std::setw( 10 ) << pars[ Par::p2 ] ); 
  }
}

void MuonCalibrationFit::CopyToBestFitParameters() {
  bool overwrite = false;
  if( m_FitType == "Chi2" ) {
    overwrite = ( g_Chi2 < g_best_Chi2 || TMath::AreEqualAbs( g_Chi2, g_best_Chi2, 0.01 ) );
  }
  else {
    overwrite = ( g_NLL < g_best_NLL || TMath::AreEqualAbs( g_NLL, g_best_NLL, 0.01 ) );
  }
  ATH_MSG_DEBUG( "overwrite flag: " << overwrite );
  ATH_MSG_DEBUG( "chi2: " << g_Chi2 );
  ATH_MSG_DEBUG( "best chi2: " << g_best_Chi2 );
  ATH_MSG_DEBUG( "first part: " << ( g_Chi2 < g_best_Chi2 ) );
  ATH_MSG_DEBUG( "second part: " << TMath::AreEqualAbs( g_Chi2, g_best_Chi2, 0.01 ) );
  if( m_RequireErrors ) {
    bool can_overwrite = false;
    for( int index = 0; index < Par::N; index++ ) {
      if( ( g_errs_low[ index ] > 0 ) || ( g_errs_up[ index ] > 0 ) ) can_overwrite = true;
    }
    if( ! can_overwrite ) overwrite = false;
  }
  if( overwrite ) {
    for( int index = 0; index < Par::N; index++ ) {
      g_best_pars[ index ] = g_pars[ index ];
      g_best_errs_up[ index ] = g_errs_up[ index ];
      g_best_errs_low[ index ] = g_errs_low[ index ];
    }
    g_best_Chi2 = g_Chi2;
    g_best_NLL = g_NLL;
  }
  ATH_MSG_INFO( "  Best So Far  ---> nll = " << std::setw( 10 ) << g_best_NLL << " | chi2 = " << std::setw( 10 ) << g_best_Chi2 << " | s0 = " << std::setw( 10 ) << g_best_pars[ Par::s0 ] << " +" << g_best_errs_up[ Par::s0 ] << " -" << g_best_errs_low[ Par::s0 ] << " | s1 = " << std::setw( 10 ) << g_best_pars[ Par::s1 ] << " +" << g_best_errs_up[ Par::s1 ] << " -" << g_best_errs_low[ Par::s1 ] << " | p0 = " << std::setw( 10 ) << g_best_pars[ Par::p0 ] << " +" << g_best_errs_up[ Par::p0 ] << " -" << g_best_errs_low[ Par::p0 ] << " | p1 = " << std::setw( 10 ) << g_best_pars[ Par::p1 ] << " +" << g_best_errs_up[ Par::p1 ] << " -" << g_best_errs_low[ Par::p1 ] << " | p2 = " << std::setw( 10 ) << g_best_pars[ Par::p2 ] << " +" << g_best_errs_up[ Par::p2 ] << " -" << g_best_errs_low[ Par::p2 ] ); 
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
    if( m_Use[ index ] ) {
      fitRegion->par[ index ] = g_best_pars[ index ];
      fitRegion->err_up[ index ] = g_best_errs_up[ index ];
      fitRegion->err_low[ index ] = g_best_errs_low[ index ];
    }
    else {
      fitRegion->par[ index ] = m_Prev[ index ];
    }
  }
  fitRegion->Corrected = kTRUE;
  fitRegion->SaveToFile( g_Output, g_Chi2, g_NLL, g_CurrentTemplate->GetDegreesOfFreedom() );
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
  //::: Resetting Initial Values to Best Values Found so Far!
  for( int index = 0; index < Par::N; index++ ) {
    if( g_best_NLL < 1E8 || g_best_Chi2 < 1E8 ) { 
      m_Init[ index ] = g_best_pars[ index ];
    }
  }
  //:::
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
      //if( g_FitTrial > 0 ) {
      //::: Reset initial values of the parameters only if not first local iteration
      if( fit_trial > 0 ) {
        for( int index = 0; index < Par::N; index++ ) {
          m_Init[ index ] = random_value.Uniform( m_MinInit[ index ], m_MaxInit[ index ] );
          m_Step[ index ] = random_value.Uniform( m_MinStep[ index ], m_MaxStep[ index ] );
        }
      }
      SetParameters( MinSimplex );
      ATH_MSG_INFO( "fit_trial = " << fit_trial << ", ndof: " << g_CurrentTemplate->GetDegreesOfFreedom() );
      successful_fit = ( MinSimplex->Minimize() && TMath::Prob( MinSimplex->MinValue(), g_CurrentTemplate->GetDegreesOfFreedom() ) > m_ProbThreshold );
      ATH_MSG_VERBOSE( "chi2_prob: " << TMath::Prob( MinSimplex->MinValue(), g_CurrentTemplate->GetDegreesOfFreedom() ) );
      fit_trial++;
      g_FitTrial++;
    }
    const double *fit_pars = MinSimplex->X();
    const double *fit_errors = MinSimplex->Errors();
    double pars[ Par::N ];
    double errors[ Par::N ];
    FoldParameters( fit_pars, fit_errors, pars, errors );
    ATH_MSG_INFO( "Simplex minimum found (no errors given):" );
    for( int index = 0; index < Par::N; index++ ) {
      ATH_MSG_INFO( "  " << m_ParNames[ index ] << " = " << std::setw( 15 ) << std::left << pars[ index ] );
    }
    if( m_DoNLL )  ATH_MSG_INFO( " nll = " << std::setw( 15 ) << std::left << MinSimplex->MinValue() ); 
    if( m_DoChi2 ) ATH_MSG_INFO( "chi2 = " << std::setw( 15 ) << std::left << MinSimplex->MinValue() ); 
    UpdateGlobalValues( MinSimplex->MinValue(), pars, errors );
    if( m_MonitoringPlots ) DoMonitoringPlots( g_pars );
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
      SetParameters( MinMigrad );
      successful_fit = MinMigrad->Minimize(); 
      fit_trial++;
    }
    const double *fit_pars = MinMigrad->X();
    const double *fit_errors = MinMigrad->Errors();
    double pars[ Par::N ];
    double errors[ Par::N ];
    FoldParameters( fit_pars, fit_errors, pars, errors );
    ATH_MSG_INFO( "Migrad minimum found:" );
    for( int index = 0; index < Par::N; index++ ) {
      ATH_MSG_INFO( "  " << m_ParNames[ index ] << " = " << std::setw( 15 ) << std::left << pars[ index ] << " +/- " << std::setw( 15 ) << std::left << errors[ index ] );
    }
    if( m_DoNLL )  ATH_MSG_INFO( " nll = " << std::setw( 15 ) << std::left << MinMigrad->MinValue() ); 
    if( m_DoChi2 ) ATH_MSG_INFO( "chi2 = " << std::setw( 15 ) << std::left << MinMigrad->MinValue() ); 
    UpdateGlobalValues( MinMigrad->MinValue(), pars, errors );
    if( m_MonitoringPlots ) DoMonitoringPlots( g_pars );
    g_CurrentTemplate->Write( "PostMigradFit" ); 
    SaveParameters( "PostMigradFit", pars, errors );
    for( int index = 0; index < Par::N; index++ ) {
      m_Init[ index ] = pars[ index ];
    }
    return kTRUE;
  }
  else if( what == "InitScan" ) { 
    ROOT::Math::Functor Fun( this, ( m_DoChi2 ) ? &MuonCalibrationFit::GetChiSquare : &MuonCalibrationFit::GetNegLogLikelihood, Par::N ); 
    ROOT::Math::Minimizer* MinInitScan = ROOT::Math::Factory::CreateMinimizer( "Minuit2", "Scan" );
    MinInitScan->SetPrintLevel( 0 );
    MinInitScan->SetStrategy( 2 );
    MinInitScan->SetTolerance( 1 );
    MinInitScan->SetMaxFunctionCalls( 100000 );
    MinInitScan->SetFunction( Fun );
    MinInitScan->SetErrorDef( m_Error );
    SetParameters( MinInitScan, true );
    MinInitScan->Minimize();
    const double *fit_pars = MinInitScan->X();
    const double *fit_errors = MinInitScan->Errors();
    double pars[ Par::N ];
    double errors[ Par::N ];
    FoldParameters( fit_pars, fit_errors, pars, errors );
    ATH_MSG_INFO( "InitScan minimum found (no errors given):" );
    for( int index = 0; index < Par::N; index++ ) {
      ATH_MSG_INFO( "  " << m_ParNames[ index ] << " = " << std::setw( 15 ) << std::left << pars[ index ] );
    }
    if( m_DoNLL )  ATH_MSG_INFO( " nll = " << std::setw( 15 ) << std::left << MinInitScan->MinValue() ); 
    if( m_DoChi2 ) ATH_MSG_INFO( "chi2 = " << std::setw( 15 ) << std::left << MinInitScan->MinValue() ); 
    UpdateGlobalValues( MinInitScan->MinValue(), pars, errors );
    if( m_MonitoringPlots ) DoMonitoringPlots( g_pars );
    g_CurrentTemplate->Write( "PostInitScanFit" ); 
    SaveParameters( "PostInitScanFit", pars, errors );
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
        if( ! m_UseLocally[ var_index ] ) continue; // It means the variable is turned off in the fit, no need to scan it!
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
          double TempMin = GetFunction( temp_pars );
          double Dist = TempMin - g_GlobalMinimum;
          //::: if new minimum (deltachi2 < 1) is found 
          if( Dist < -1 ) {
            for( int index = 0; index < Par::N; index++ ) {
              m_Init[ index ] = temp_pars[ index ];
            }
            return kFALSE;
          }
          //::: if between 2 and 3 sigmas on one side
          else if( Dist > 4 * m_Error && Dist < 9 * m_Error ) {
            found = kTRUE;
            final_alpha_pos[ var_index ] = alpha_pos[ var_index ];
            ATH_MSG_DEBUG( "Eureka: alpha_pos[ " << var_index << " ] = " << alpha_pos[ var_index ] << "  ===>  pars[ var_index ] = " << pars[ var_index ] * ( 1. + alpha_pos[ var_index ] ) );
          }
          else {
            float sf = sqrt( random_value.Uniform( 4 * m_Error, 9 * m_Error ) / TMath::Abs( Dist ) );
            alpha_pos[ var_index ] *= sf;
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
          double TempMin = GetFunction( temp_pars );
          double Dist = TempMin - g_GlobalMinimum;
          //::: if new minimum (deltachi2 < 1) is found 
          if( Dist < -1 ) {
            for( int index = 0; index < Par::N; index++ ) {
              m_Init[ index ] = temp_pars[ index ];
            }
            return kFALSE;
          }
          //::: if between 2 and 3 sigmas on one side
          if( Dist > 4 * m_Error && Dist < 9 * m_Error ) {
            found = kTRUE;
            final_alpha_neg[ var_index ] = alpha_neg[ var_index ];
            ATH_MSG_DEBUG( "Eureka: alpha_neg[ " << var_index << " ] = " << alpha_neg[ var_index ] << "  ===>  pars[ var_index ] = " << pars[ var_index ] * ( 1. - alpha_neg[ var_index ] ) );
          }
          else {
            alpha_neg[ var_index ] *= sqrt( random_value.Uniform( 4 * m_Error, 9 * m_Error ) / TMath::Abs( Dist ) );
            if( m_PosDef[ var_index ] && pars[ var_index ] * ( 1. - alpha_neg[ var_index ] ) < 0 ) {
              ATH_MSG_DEBUG( "Ouch! alpha_neg = " << alpha_neg[ var_index ] << ", temp_pars[ var_index ] = " << pars[ var_index ] * ( 1. - alpha_neg[ var_index ] ) );
              found = kTRUE;
              alpha_neg[ var_index ] = 1.; 
              final_alpha_neg[ var_index ] = alpha_neg[ var_index ];
              ATH_MSG_DEBUG( "Eureka: alpha_neg[ " << var_index << " ] = " << alpha_neg[ var_index ] << "  ===>  pars[ var_index ] = " << pars[ var_index ] * ( 1. - alpha_neg[ var_index ] ) );
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
        if( ! m_UseLocally[ the_index ] ) {
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
          NewMinimumFound = ( currentY < g_GlobalMinimum && ! TMath::AreEqualAbs( currentY, g_GlobalMinimum, 0.3 ) ); 
          ATH_MSG_DEBUG( "currentY = " << currentY << "  g_GlobalMinimum = " << g_GlobalMinimum << "  NewMinimumFound = " << NewMinimumFound );
          if( NewMinimumFound ) return kFALSE;
          //::: Case 2: previous sampling > m_Error and current < m_Error
          if( previousY - g_GlobalMinimum > m_Error && currentY - g_GlobalMinimum < m_Error ) {
            double interY = g_GlobalMinimum + m_Error;
            double interX = ( currentX * ( previousY - interY ) + previousX * ( interY - currentY ) ) / ( previousY - currentY );
            error_left = fabs( interX - pars[ the_index ] );
          }
          //::: Case 3: previous sampling < m_Error and current > m_Error
          if( previousY - g_GlobalMinimum < m_Error && currentY - g_GlobalMinimum > m_Error ) {
            double interY = g_GlobalMinimum + m_Error;
            double interX = ( currentX * ( previousY - interY ) + previousX * ( interY - currentY ) ) / ( previousY - currentY );
            error_right = fabs( interX - pars[ the_index ] );
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
        if( ! m_UseLocally[ the_index ] ) continue;
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
        if( m_MonitoringPlots ) DoMonitoringPlots( g_pars );
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
      float pt_for_binning = ( theInfo.GetMass() < 10 ) ? std::min( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ) : std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt );
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), theInfo.First_Mu_pt );
      //:::
      //::: Method 2: pt bin chosen with leading muon (Z), subleading muon (Jpsi)!
      g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), pt_for_binning );
      //:::
      //::: Method 3: use only events where leading muon is in RoF!
      //if( theInfo.First_Mu_pt > theInfo.Second_Mu_pt ) {
      //  g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), theInfo.First_Mu_pt );
      //}
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::Data, theInfo.GetRho().first, theInfo.First_Mu_pt );
    }
    BOOST_FOREACH( Tools::Info theInfo, *g_DataBothInfo ) {
      if( ! theInfo.IsOS ) continue;
      float pt_for_binning = ( theInfo.GetMass() < 10 ) ? std::min( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ) : std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt );
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), theInfo.First_Mu_pt );
      //g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), theInfo.Second_Mu_pt );
      //::: Method 2/3: pt bin chosen with leading muon / use only events where leading muon is in RoF
      //theInfo.Print( Type::Data );
      g_CurrentTemplate->FillMass( Type::Data, theInfo.GetMass(), pt_for_binning );
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::Data, theInfo.GetRho().first, theInfo.First_Mu_pt );
      g_CurrentTemplate->FillRho( Type::Data, theInfo.GetRho().second, theInfo.Second_Mu_pt );
    }
    //::: Background
    g_CurrentTemplate->ClearBackground();
    BOOST_FOREACH( Tools::Info theInfo, *g_BackgroundSingleInfo ) {
      if( ! theInfo.IsOS ) continue;
      float pt_for_binning = ( theInfo.GetMass() < 10 ) ? std::min( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ) : std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt );
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), theInfo.First_Mu_pt, theInfo.Weight );
      //::: Method 2: pt bin chosen with leading muon!
      g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), pt_for_binning, theInfo.Weight );
      //::: Method 3: use only events where leading muon is in RoF!
      //if( theInfo.First_Mu_pt > theInfo.Second_Mu_pt ) {
      //  g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), theInfo.First_Mu_pt, theInfo.Weight );
      //}
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::Background, theInfo.GetRho().first, theInfo.First_Mu_pt, theInfo.Weight );
    }
    BOOST_FOREACH( Tools::Info theInfo, *g_BackgroundBothInfo ) {
      if( ! theInfo.IsOS ) continue;
      float pt_for_binning = ( theInfo.GetMass() < 10 ) ? std::min( theInfo.First_Mu_pt, theInfo.Second_Mu_pt ) : std::max( theInfo.First_Mu_pt, theInfo.Second_Mu_pt );
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), theInfo.First_Mu_pt, theInfo.Weight );
      //g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), theInfo.Second_Mu_pt, theInfo.Weight );
      //::: Method 2/3: pt bin chosen with leading muon!
      g_CurrentTemplate->FillMass( Type::Background, theInfo.GetMass(), pt_for_binning, theInfo.Weight );
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
  TRandom3 rand_gen( 2988100123 );
  BOOST_FOREACH( Tools::Info theInfo, *g_MonteCarloSingleInfo ) {
    if( ! theInfo.IsOS ) continue;
    //std::cout << "GREP ME ====== " << int( TMath::Abs( theInfo.Gamma ) * 1000000 ) << std::endl;
    for( int index = 0; index < m_NumRandomValues; index++ ) {
      double rnd = rand_gen.Gaus( 0, 1 );
      float pt_for_binning = ( theInfo.GetMass() < 10 ) ? std::min( theInfo.Corr_First_Mu_pt( par, rnd ), theInfo.Second_Mu_pt ) : std::max( theInfo.Corr_First_Mu_pt( par, rnd ), theInfo.Second_Mu_pt );
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassSingle( par, rnd ), theInfo.Corr_First_Mu_pt( par, rnd ), theInfo.Weight / ( float ) m_NumRandomValues );
      //::: Method 2: pt bin chosen with leading muon!
      g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassSingle( par, rnd ), pt_for_binning, theInfo.Weight / ( float ) m_NumRandomValues );
      //::: Method 3: use only events where leading muon is in RoF!
      //if( theInfo.Corr_First_Mu_pt( par, rnd ) > theInfo.Second_Mu_pt ) {
      //  g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassSingle( par, rnd ), theInfo.Corr_First_Mu_pt( par, rnd ), theInfo.Weight / ( float ) m_NumRandomValues );
      //}
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::MC, theInfo.GetRho( par, rnd ).first, theInfo.Corr_First_Mu_pt( par, rnd ), theInfo.Weight / ( float ) m_NumRandomValues );
    }
  }
  //::: Double Muon Correction
  BOOST_FOREACH( Tools::Info theInfo, *g_MonteCarloBothInfo ) {
    if( ! theInfo.IsOS ) continue;
    //std::cout << "GREP ME ====== " << int( TMath::Abs( theInfo.Gamma ) * 1000000 ) << std::endl;
    //TRandom2 rand_gen( int( TMath::Abs( theInfo.Gamma ) * 1000000 ) );
    for( int index = 0; index < m_NumRandomValues; index++ ) {
      double rnd1 = rand_gen.Gaus( 0, 1 );
      double rnd2 = rand_gen.Gaus( 0, 1 );
      float pt_for_binning = ( theInfo.GetMass() < 10 ) ? std::min( theInfo.Corr_First_Mu_pt( par, rnd1 ), theInfo.Corr_Second_Mu_pt( par, rnd2 ) ) : std::max( theInfo.Corr_First_Mu_pt( par, rnd1 ), theInfo.Corr_Second_Mu_pt( par, rnd2 ) ); 
      //::://::://:::
      //::: Mass
      //::: Method 1: pt bin chosen with muon to be corrected
      //g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassBoth( par, rnd1, rnd2 ), theInfo.Corr_First_Mu_pt( par, rnd1 ), theInfo.Weight / ( float ) m_NumRandomValues );
      //g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassBoth( par, rnd1, rnd2 ), theInfo.Corr_Second_Mu_pt( par, rnd2 ), theInfo.Weight / ( float ) m_NumRandomValues );
      //if( theInfo.EvtNum == 911 || theInfo.EvtNum == 991 || theInfo.EvtNum == 239 || theInfo.EvtNum == 144 || theInfo.EvtNum == 68 ) {
      //  std::cout << " evt: " << theInfo.EvtNum << std::endl; 
      //  std::cout << " bef: " << theInfo.First_Mu_pt << ", aft: " << theInfo.Corr_First_Mu_pt( par, rnd1 ) << std::endl;
      //  std::cout << " bef: " << theInfo.Second_Mu_pt << ", aft: " << theInfo.Corr_Second_Mu_pt( par, rnd2 ) << std::endl;
      //  std::cout << " pars: ";
      //  for( int i = 0; i < Par::N; i++ ) std::cout << "( " << i << "," << par[i] << ") ";
      //  std::cout << std::endl;
      //  std::cout << theInfo.GetMassBoth( par, rnd1, rnd2 ) << std::endl;
      //  std::cout << pt_for_binning << std::endl;
      //}
      //::: Method 2: pt bin chosen with leading muon!
      //std::cout << " evt: " << theInfo.EvtNum << std::endl; 
      g_CurrentTemplate->FillMass( Type::MC, theInfo.GetMassBoth( par, rnd1, rnd2 ), pt_for_binning, theInfo.Weight / ( float ) m_NumRandomValues );
      //::://::://:::
      //::: Rho 
      g_CurrentTemplate->FillRho( Type::MC, theInfo.GetRho( par, rnd1 ).first, theInfo.Corr_First_Mu_pt( par, rnd1 ), theInfo.Weight / ( float ) m_NumRandomValues );
      g_CurrentTemplate->FillRho( Type::MC, theInfo.GetRho( par, rnd2 ).second, theInfo.Corr_Second_Mu_pt( par, rnd2 ), theInfo.Weight / ( float ) m_NumRandomValues );
    }
  }
  g_UpdatedTemplates++;
}

void MuonCalibrationFit::FoldParameters( const double* fit_pars, const double* fit_errs, double* pars, double* errs ) {
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

void MuonCalibrationFit::ResetParameters() {
  //::: Go back to general configuration for parameters to be fit
  for( int index_minuit = 0; index_minuit < Par::N; index_minuit++ ) {
    m_UseLocally[ index_minuit ] = m_Use[ index_minuit ];
  }
  return;
}

void MuonCalibrationFit::SetParameters( ROOT::Math::Minimizer* Minim, bool forScan ) {
  std::vector< unsigned int > ordered_indeces;
  // if( forScan ) {
  //   ordered_indeces = { Par::p0, Par::p1, Par::p2, Par::s0, Par::s1 };
  // }
  // else {
  ordered_indeces = { Par::s0, Par::s1, Par::p0, Par::p1, Par::p2 };
  // }
  for( int index_minuit = 0; index_minuit < Par::N; index_minuit++ ) {
    int index = ordered_indeces[ index_minuit ];
    if( ! m_UseLocally[ index ] ) {
      float fixed_value = g_best_pars[ index ];
      if( ! m_Use[ index ] ) {
        //::: If not generally used in the fit:
        fixed_value = m_Init[ index ];
      }
      ATH_MSG_INFO( "Setting Constant Variable Named: " << m_ParNames[ index ] << " at: " << fixed_value );
      Minim->SetFixedVariable( index_minuit, m_ParNames[ index ], fixed_value ); 
    }
    else {
      float ScaleFactorForScan = ( forScan ) ? 5 : 1;
      ATH_MSG_INFO( "Setting Variable Named: " << m_ParNames[ index ] << ", Init: " << m_Init[ index ] << ", Step: " << m_Step[ index ] * ScaleFactorForScan );
      Minim->SetVariable( index_minuit, m_ParNames[ index ], m_Init[ index ], m_Step[ index ] * ScaleFactorForScan );
    }
  }
}

/*
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
*/
