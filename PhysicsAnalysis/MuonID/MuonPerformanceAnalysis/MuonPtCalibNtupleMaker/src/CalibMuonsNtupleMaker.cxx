/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//::: STL includes
#include <string>

//::: Local includes
#include "MuonPtCalibNtupleMaker/CalibMuonsNtupleMaker.h"

CalibMuonsNtupleMaker::CalibMuonsNtupleMaker( const std::string& name, ISvcLocator* pSvcLocator ) : ::AthAlgorithm( name, pSvcLocator ), m_SelectorTool( "CalibMuonsSelTool", this ), m_HistSvc( "THistSvc", name ) {

  declareProperty( "SelectorTool", m_SelectorTool );
  declareProperty( "ExtendedVersion", m_Extended = false );
 
}

StatusCode CalibMuonsNtupleMaker::initialize() {

  ATH_MSG_INFO( "Initializing " << name() << "..." );

  StatusCode sc = m_SelectorTool.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve AlgTool " << m_SelectorTool );
    return sc;
  }

  sc = m_HistSvc.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve HistSvc" );
    return sc;
  }

  m_OutputTree = new TTree( "MuonMomentumCalibrationTree", "" );

  m_OutputTree->Branch( "RunNumber",     &m_RunNumber,     "RunNumber/i"     );
  m_OutputTree->Branch( "EvtNumber",     &m_EvtNumber,     "EvtNumber/l"     );
  m_OutputTree->Branch( "LumiBlock",     &m_LumiBlock,     "LumiBlock/i"     );
  m_OutputTree->Branch( "ChannelNumber", &m_ChannelNumber, "ChannelNumber/i" );
  m_OutputTree->Branch( "EventWeight",   &m_EventWeight,   "EventWeight/F"   );

  m_OutputTree->Branch( "Pos_Pt",      &m_Pos_Pt,      "Pos_Pt/F"      );
  m_OutputTree->Branch( "Pos_Eta",     &m_Pos_Eta,     "Pos_Eta/F"     );
  m_OutputTree->Branch( "Pos_Phi",     &m_Pos_Phi,     "Pos_Phi/F"     );
  m_OutputTree->Branch( "Pos_Charge",  &m_Pos_Charge,  "Pos_Charge/F"  );
  m_OutputTree->Branch( "Pos_ID_Pt",   &m_Pos_ID_Pt,   "Pos_ID_Pt/F"   );
  m_OutputTree->Branch( "Pos_MS_Pt",   &m_Pos_MS_Pt,   "Pos_MS_Pt/F"   );
  m_OutputTree->Branch( "Pos_MS_Eta",  &m_Pos_MS_Eta,  "Pos_MS_Eta/F"  );
  m_OutputTree->Branch( "Pos_MS_Phi",  &m_Pos_MS_Phi,  "Pos_MS_Phi/F"  );
  m_OutputTree->Branch( "Pos_Quality", &m_Pos_Quality, "Pos_Quality/I" );

  m_OutputTree->Branch( "Neg_Pt",      &m_Neg_Pt,      "Neg_Pt/F"      );
  m_OutputTree->Branch( "Neg_Eta",     &m_Neg_Eta,     "Neg_Eta/F"     );
  m_OutputTree->Branch( "Neg_Phi",     &m_Neg_Phi,     "Neg_Phi/F"     );
  m_OutputTree->Branch( "Neg_Charge",  &m_Neg_Charge,  "Neg_Charge/F"  );
  m_OutputTree->Branch( "Neg_ID_Pt",   &m_Neg_ID_Pt,   "Neg_ID_Pt/F"   );
  m_OutputTree->Branch( "Neg_MS_Pt",   &m_Neg_MS_Pt,   "Neg_MS_Pt/F"   );
  m_OutputTree->Branch( "Neg_MS_Eta",  &m_Neg_MS_Eta,  "Neg_MS_Eta/F"  );
  m_OutputTree->Branch( "Neg_MS_Phi",  &m_Neg_MS_Phi,  "Neg_MS_Phi/F"  );
  m_OutputTree->Branch( "Neg_Quality", &m_Neg_Quality, "Neg_Quality/I" );

  sc = m_HistSvc->regTree( "/MCPCALIB/MuonMomentumCalibrationTree", m_OutputTree );
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to register output TTree" );
    return sc;
  }

  m_CutFlow = new TH1F( "CutFlow", "", 20, 0, 20 );

  sc = m_HistSvc->regHist( "/MCPCALIB/CutFlow", m_CutFlow );
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to register cut flow histogram" );
    return sc;
  }

  return StatusCode::SUCCESS;

}

StatusCode CalibMuonsNtupleMaker::finalize() {

  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  return StatusCode::SUCCESS;

}

StatusCode CalibMuonsNtupleMaker::execute() {  

  ATH_MSG_DEBUG( "Executing " << name() << "..." );

  const xAOD::MuonContainer* inputMuons = 0;
  StatusCode sc = evtStore()->retrieve( inputMuons, "Muons" );
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve MuonContainer with key: Muons" );
    return StatusCode::FAILURE;
  }

  PairsVector outputMuons = m_SelectorTool->GetMuons( inputMuons, m_CutFlow );

  ATH_MSG_DEBUG( "In " << name() << ", vector of pairs size: " << outputMuons.size() );
 
  const xAOD::EventInfo* evtInfo = 0;
  sc = evtStore()->retrieve( evtInfo, "EventInfo" );
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve EventInfo object" );
    return StatusCode::FAILURE;
  }

  m_RunNumber = evtInfo->runNumber();
  m_EvtNumber = evtInfo->eventNumber();
  m_LumiBlock = evtInfo->lumiBlock();
  m_ChannelNumber = ( evtInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ) ? evtInfo->mcChannelNumber() : 0;
  m_EventWeight = ( evtInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ) ? evtInfo->mcEventWeight() : 1.;
 
  for( auto pair: outputMuons ) { 

    m_Pos_Pt = ( pair.first )->pt() / CLHEP::GeV;
    m_Pos_Eta = ( pair.first )->eta();
    m_Pos_Phi = ( pair.first )->phi();
    m_Pos_Charge = ( pair.first )->charge();
    //:::
    ATH_MSG_DEBUG( "Retrieving Positive ID Track..." );
    const ElementLink< xAOD::TrackParticleContainer >& pos_idTrack = ( pair.first )->inDetTrackParticleLink();
    ATH_MSG_DEBUG( "Positive ID Track: " << *pos_idTrack );
    m_Pos_ID_Pt = ( *pos_idTrack )->pt() / CLHEP::GeV;
    //:::
    ATH_MSG_DEBUG( "Retrieving Positive MS Track..." );
    const ElementLink< xAOD::TrackParticleContainer >& pos_msTrack = ( pair.first )->muonSpectrometerTrackParticleLink();
    ATH_MSG_DEBUG( "Positive MS Track: " << *pos_msTrack );
    m_Pos_MS_Pt = ( *pos_msTrack )->pt() / CLHEP::GeV;
    m_Pos_MS_Eta = ( *pos_msTrack )->eta();
    m_Pos_MS_Phi = ( *pos_msTrack )->phi();
    m_Pos_Quality = ( pair.first )->auxdata< int >( "MST_Quality" );  
    //::://:::
    m_Neg_Pt = ( pair.second )->pt() / CLHEP::GeV;
    m_Neg_Eta = ( pair.second )->eta();
    m_Neg_Phi = ( pair.second )->phi();
    m_Neg_Charge = ( pair.second )->charge();
    //:::
    ATH_MSG_DEBUG( "Retrieving Negative ID Track..." );
    const ElementLink< xAOD::TrackParticleContainer >& neg_idTrack = ( pair.second )->inDetTrackParticleLink();
    ATH_MSG_DEBUG( "Negative ID Track: " << *neg_idTrack );
    m_Neg_ID_Pt = ( *neg_idTrack )->pt() / CLHEP::GeV;
    //:::
    ATH_MSG_DEBUG( "Retrieving Negative MS Track..." );
    const ElementLink< xAOD::TrackParticleContainer >& neg_msTrack = ( pair.second )->muonSpectrometerTrackParticleLink();
    ATH_MSG_DEBUG( "Negative MS Track: " << *neg_msTrack );
    m_Neg_MS_Pt = ( *neg_msTrack )->pt() / CLHEP::GeV;
    m_Neg_MS_Eta = ( *neg_msTrack )->eta();
    m_Neg_MS_Phi = ( *neg_msTrack )->phi();
    m_Neg_Quality = ( pair.second )->auxdata< int >( "MST_Quality" );  

    m_OutputTree->Fill();

  }

  return StatusCode::SUCCESS;

}
