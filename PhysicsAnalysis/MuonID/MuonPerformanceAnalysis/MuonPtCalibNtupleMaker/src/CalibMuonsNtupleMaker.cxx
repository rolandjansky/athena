/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//::: Local includes
#include "MuonPtCalibNtupleMaker/CalibMuonsNtupleMaker.h"

#define DEBUG_COUT std::cout << " I got this far ---> " << __FILE__ << ": " << __LINE__ << std::endl;

CalibMuonsNtupleMaker::CalibMuonsNtupleMaker( const std::string& name, ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_SelectorTool( "CalibMuonsSelTool", this ),
  m_TrigDecisionTool( "Trig::TrigDecisionTool/TrigDecisionTool" ),
  m_TrigMuonMatching( "Trig::TrigMuonMatching/TrigMuonMatching" ), 
  m_HistSvc( "THistSvc", name ),
  m_CutFlow( NULL ), m_OutputTree( NULL ),
  m_Event( NULL ), m_TrigInfo( NULL ) {

  declareProperty( "TrigDecisionTool", m_TrigDecisionTool );
  declareProperty( "TrigMuonMatching", m_TrigMuonMatching );
  declareProperty( "SelectorTool", m_SelectorTool );
  declareProperty( "ExtendedVersion", m_Extended = false );
 
}

StatusCode CalibMuonsNtupleMaker::initialize() {

  ATH_MSG_INFO( "Initializing " << name() << "..." );

  ATH_CHECK( m_SelectorTool.retrieve() );
  ATH_CHECK( m_HistSvc.retrieve() );
  ATH_CHECK( m_TrigDecisionTool.retrieve() );
  ATH_CHECK( m_TrigMuonMatching.retrieve() );

  m_OutputTree = new TTree( "MuonMomentumCalibrationTree", "" );

  m_Types.push_back( "Pos" );
  m_Types.push_back( "Neg" );

  m_Event = new EvtInfo();
  m_TrigInfo = new TrigInfo();
  for( auto& Type: m_Types ) {
    m_Combined[ Type ] = new TrackInfo( "CB" );
    m_InnerDet[ Type ] = new TrackInfo( "ID" );
    m_MSExtr  [ Type ] = new TrackInfo( "ME" );
    m_Hits    [ Type ] = new HitsInfo();
    m_MuonInfo[ Type ] = new MuonInfo();
  }

  m_Event->Register( m_OutputTree );
  m_TrigInfo->Register( m_OutputTree );
  for( auto& Type: m_Types ) {
    m_Combined[ Type ]->Register( m_OutputTree, Type );
    m_InnerDet[ Type ]->Register( m_OutputTree, Type );
    m_MSExtr  [ Type ]->Register( m_OutputTree, Type );
    m_Hits    [ Type ]->Register( m_OutputTree, Type );
    m_MuonInfo[ Type ]->Register( m_OutputTree, Type );
  }

  ATH_CHECK( m_HistSvc->regTree( "/MCPCALIB/MuonMomentumCalibrationTree", m_OutputTree ) );

  m_CutFlow = new TH1F( "CutFlow", "", 20, 0, 20 );

  ATH_CHECK( m_HistSvc->regHist( "/MCPCALIB/CutFlow", m_CutFlow ) );

  return StatusCode::SUCCESS;

}

StatusCode CalibMuonsNtupleMaker::finalize() {

  ATH_MSG_INFO( "Finalizing " << name() << "..." );

  delete m_Event;
  delete m_TrigInfo;
  for( auto& Type: m_Types ) {
    delete m_Combined[ Type ];
    delete m_InnerDet[ Type ];
    delete m_MSExtr  [ Type ];
    delete m_Hits    [ Type ];
    delete m_MuonInfo[ Type ];
  }

  return StatusCode::SUCCESS;

}

StatusCode CalibMuonsNtupleMaker::execute() {  

  ATH_MSG_DEBUG( "Executing " << name() << "..." );

  m_Event->Reset();
  m_TrigInfo->Reset();
  for( auto& Type: m_Types ) {
    m_Combined[ Type ]->Reset();
    m_InnerDet[ Type ]->Reset();
    m_MSExtr  [ Type ]->Reset();
    m_Hits    [ Type ]->Reset();
    m_MuonInfo[ Type ]->Reset();
  }

  m_PrimaryVertex = NULL;
  const xAOD::VertexContainer* vertexContainer = 0;
  StatusCode sc = evtStore()->retrieve( vertexContainer, "PrimaryVertices" );
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve VertexContainer with key: PrimaryVertices" );
    return StatusCode::FAILURE;
  }

  for( xAOD::VertexContainer::const_iterator pv_itr = vertexContainer->begin(); pv_itr != vertexContainer->end(); ++pv_itr ) {
    if( ( *pv_itr )->vertexType() == xAOD::VxType::PriVtx ) {
      m_PrimaryVertex = *pv_itr;
    }
  }
  if( m_PrimaryVertex == NULL ) return StatusCode::SUCCESS;

  const xAOD::MuonContainer* inputMuons = 0;
  sc = evtStore()->retrieve( inputMuons, "Muons" );
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve MuonContainer with key: Muons" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "MuonContainer: " << inputMuons );

  PairsVector outputMuons = m_SelectorTool->GetMuons( inputMuons, m_CutFlow );

  ATH_MSG_DEBUG( "In " << name() << ", vector of pairs size: " << outputMuons.size() );
 
  m_EvtInfo = NULL;
  sc = evtStore()->retrieve( m_EvtInfo, "EventInfo" );
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve EventInfo object" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "EventInfo: " << m_EvtInfo );

  m_Event->Fill( m_EvtInfo );
  m_TrigInfo->Fill( m_TrigDecisionTool );
    
  for( auto pair: outputMuons ) { 

    for( auto& Type: m_Types ) {
      m_Combined[ Type ]->Reset();
      m_InnerDet[ Type ]->Reset();
      m_MSExtr  [ Type ]->Reset();
      m_Hits    [ Type ]->Reset();
      m_MuonInfo[ Type ]->Reset();
    }

    m_TrigInfo->FillMatching( m_TrigMuonMatching, pair.first, pair.second );

    ATH_MSG_DEBUG( "First Muon: " << pair.first );
    ATH_MSG_DEBUG( "Second Muon: " << pair.second );

    std::map< std::string, const xAOD::Muon* > pair_map;
    pair_map[ "Pos" ] = pair.first;
    pair_map[ "Neg" ] = pair.second;
   
    for( auto& Type: m_Types ) {
    
      ATH_MSG_DEBUG( "Retrieving CB Track [" << Type << "]..." );
      const xAOD::TrackParticle* cbTrack = pair_map[ Type ]->trackParticle( xAOD::Muon::CombinedTrackParticle );
      m_Combined[ Type ]->Fill( cbTrack );

      ATH_MSG_DEBUG( "Retrieving ID Track [" << Type << "]..." );
      const xAOD::TrackParticle* idTrack = pair_map[ Type ]->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
      m_InnerDet[ Type ]->Fill( idTrack );

      ATH_MSG_DEBUG( "Retrieving ME Track [" << Type << "]..." );
      const xAOD::TrackParticle* meTrack;
      try {
        meTrack = pair_map[ Type ]->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
      } 
      catch( SG::ExcBadAuxVar b ) {
        meTrack = pair_map[ Type ]->trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
      }
      m_MSExtr[ Type ]->Fill( meTrack );

      m_Hits[ Type ]->Fill( pair_map[ Type ] );

      m_MuonInfo[ Type ]->Fill( pair_map[ Type ], m_PrimaryVertex, m_EvtInfo );

    }

    m_OutputTree->Fill();

  }

  return StatusCode::SUCCESS;

}
