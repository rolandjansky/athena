/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//::: STL includes
#include <string>

//::: Local includes
#include "MuonPtCalibNtupleMaker/CalibMuonsNtupleMaker.h"

CalibMuonsNtupleMaker::CalibMuonsNtupleMaker( const std::string& name, ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_SelectorTool( "CalibMuonsSelTool", this ),
  m_HistSvc( "THistSvc", name ),
  m_CutFlow(NULL), m_OutputTree(NULL),
  m_RunNumber(0), m_LumiBlock(0),
  m_ChannelNumber(0), m_EvtNumber(0),
  m_EventWeight(-99.),
  m_Pos_Pt(0.), m_Neg_Pt(0.),
  m_Pos_Eta(0.), m_Neg_Eta(0.),
  m_Pos_Phi(0.), m_Neg_Phi(0.),
  m_Pos_Charge(0.), m_Neg_Charge(0.),
  m_Pos_ID_Pt(0.), m_Neg_ID_Pt(0.),
  m_Pos_ME_Pt(0.), m_Neg_ME_Pt(0.),
  m_Pos_ME_Eta(0.), m_Neg_ME_Eta(0.),
  m_Pos_ME_Phi(0.), m_Neg_ME_Phi(0.),
  m_Pos_PtCone20(0.), m_Neg_PtCone20(0.),
  m_Pos_Quality(0), m_Neg_Quality(0),
  m_Pos_Author(0), m_Neg_Author(0),
  m_Pos_EnergyLossType(0), m_Neg_EnergyLossType(0),
  m_Pos_PrimarySector(0), m_Neg_PrimarySector(0),
  m_Pos_SecondarySector(0), m_Neg_SecondarySector(0),
  m_CB_Mass(0.), m_ID_Mass(0.), m_ME_Mass(0.)
{

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

  m_OutputTree->Branch( "RunNumber",     &m_RunNumber     );
  m_OutputTree->Branch( "EvtNumber",     &m_EvtNumber     );
  m_OutputTree->Branch( "LumiBlock",     &m_LumiBlock     );
  m_OutputTree->Branch( "ChannelNumber", &m_ChannelNumber );
  m_OutputTree->Branch( "EventWeight",   &m_EventWeight   );

  m_OutputTree->Branch( "Pos_Pt",              &m_Pos_Pt              );
  m_OutputTree->Branch( "Pos_Eta",             &m_Pos_Eta             );
  m_OutputTree->Branch( "Pos_Phi",             &m_Pos_Phi             );
  m_OutputTree->Branch( "Pos_Charge",          &m_Pos_Charge          );
  m_OutputTree->Branch( "Pos_ID_Pt",           &m_Pos_ID_Pt           );
  m_OutputTree->Branch( "Pos_ME_Pt",           &m_Pos_ME_Pt           );
  m_OutputTree->Branch( "Pos_ME_Eta",          &m_Pos_ME_Eta          );
  m_OutputTree->Branch( "Pos_ME_Phi",          &m_Pos_ME_Phi          );
  m_OutputTree->Branch( "Pos_PtCone20",        &m_Pos_PtCone20        );
  m_OutputTree->Branch( "Pos_Quality",         &m_Pos_Quality         );
  m_OutputTree->Branch( "Pos_Author",          &m_Pos_Author          );
  m_OutputTree->Branch( "Pos_EnergyLossType",  &m_Pos_EnergyLossType  );
  m_OutputTree->Branch( "Pos_PrimarySector",   &m_Pos_PrimarySector   );
  m_OutputTree->Branch( "Pos_SecondarySector", &m_Pos_SecondarySector );

  m_OutputTree->Branch( "Neg_Pt",              &m_Neg_Pt              );
  m_OutputTree->Branch( "Neg_Eta",             &m_Neg_Eta             );
  m_OutputTree->Branch( "Neg_Phi",             &m_Neg_Phi             );
  m_OutputTree->Branch( "Neg_Charge",          &m_Neg_Charge          );
  m_OutputTree->Branch( "Neg_ID_Pt",           &m_Neg_ID_Pt           );
  m_OutputTree->Branch( "Neg_ME_Pt",           &m_Neg_ME_Pt           );
  m_OutputTree->Branch( "Neg_ME_Eta",          &m_Neg_ME_Eta          );
  m_OutputTree->Branch( "Neg_ME_Phi",          &m_Neg_ME_Phi          );
  m_OutputTree->Branch( "Neg_PtCone20",        &m_Neg_PtCone20        );
  m_OutputTree->Branch( "Neg_Quality",         &m_Neg_Quality         );
  m_OutputTree->Branch( "Neg_Author",          &m_Neg_Author          );
  m_OutputTree->Branch( "Neg_EnergyLossType",  &m_Neg_EnergyLossType  );
  m_OutputTree->Branch( "Neg_PrimarySector",   &m_Neg_PrimarySector   );
  m_OutputTree->Branch( "Neg_SecondarySector", &m_Neg_SecondarySector );

  m_OutputTree->Branch( "CB_Mass", &m_CB_Mass );
  m_OutputTree->Branch( "ID_Mass", &m_ID_Mass );
  m_OutputTree->Branch( "ME_Mass", &m_ME_Mass );

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
  ATH_MSG_DEBUG( "MuonContainer: " << inputMuons );

  PairsVector outputMuons = m_SelectorTool->GetMuons( inputMuons, m_CutFlow );

  ATH_MSG_DEBUG( "In " << name() << ", vector of pairs size: " << outputMuons.size() );
 
  const xAOD::EventInfo* evtInfo = 0;
  sc = evtStore()->retrieve( evtInfo, "EventInfo" );
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve EventInfo object" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG( "EventInfo: " << evtInfo );

  m_RunNumber = evtInfo->runNumber();
  m_EvtNumber = evtInfo->eventNumber();
  m_LumiBlock = evtInfo->lumiBlock();
  m_ChannelNumber = ( evtInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ) ? evtInfo->mcChannelNumber() : 0;
  m_EventWeight = ( evtInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ) ? evtInfo->mcEventWeight() : 1.;
 
  for( auto pair: outputMuons ) { 

    ATH_MSG_DEBUG( "First Muon: " << pair.first );
    ATH_MSG_DEBUG( "Second Muon: " << pair.second );

    m_Pos_Pt = ( pair.first )->pt() / CLHEP::GeV;
    m_Pos_Eta = ( pair.first )->eta();
    m_Pos_Phi = ( pair.first )->phi();
    m_Pos_Charge = ( pair.first )->charge();
    //:::
    ATH_MSG_DEBUG( "Retrieving Positive ID Track..." );
    const xAOD::TrackParticle* pos_idTrack = ( pair.first )->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
    ATH_MSG_DEBUG( "Positive ID Track: " << pos_idTrack );
    m_Pos_ID_Pt = pos_idTrack->pt() / CLHEP::GeV;
    //:::
    ATH_MSG_DEBUG( "Retrieving Positive ME Track..." );
    const xAOD::TrackParticle* pos_meTrack;
    try {
      pos_meTrack = ( pair.first )->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
    } 
    catch( SG::ExcBadAuxVar b ) {
      pos_meTrack = ( pair.first )->trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
    }
    ATH_MSG_DEBUG( "Positive ME Track: " << pos_meTrack );
    m_Pos_ME_Pt =  pos_meTrack->pt() / CLHEP::GeV;
    m_Pos_ME_Eta = pos_meTrack->eta();
    m_Pos_ME_Phi = pos_meTrack->phi();
    m_Pos_Quality = ( pair.first )->auxdata< int >( "MST_Quality" );  
    m_Pos_PtCone20 = ( pair.first )->auxdata< float >( "ptcone20" );
    m_Pos_Author = ( pair.first )->auxdata< uint16_t >( "author" );
    m_Pos_EnergyLossType = ( pair.first )->auxdata< uint8_t >( "energyLossType" );
    m_Pos_PrimarySector = ( pair.first )->auxdata< uint8_t >( "primarySector" );
    m_Pos_SecondarySector = ( pair.first )->auxdata< uint8_t >( "secondarySector" );
    //::://:::
    m_Neg_Pt = ( pair.second )->pt() / CLHEP::GeV;
    m_Neg_Eta = ( pair.second )->eta();
    m_Neg_Phi = ( pair.second )->phi();
    m_Neg_Charge = ( pair.second )->charge();
    //:::
    ATH_MSG_DEBUG( "Retrieving Negative ID Track..." );
    const xAOD::TrackParticle* neg_idTrack = ( pair.second )->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
    ATH_MSG_DEBUG( "Negative ID Track: " << neg_idTrack );
    m_Neg_ID_Pt = neg_idTrack->pt() / CLHEP::GeV;
    //:::
    ATH_MSG_DEBUG( "Retrieving Negative ME Track..." );
    const xAOD::TrackParticle* neg_meTrack;
    try {
      neg_meTrack = ( pair.second )->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
    } 
    catch( SG::ExcBadAuxVar b ) {
      neg_meTrack = ( pair.second )->trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
    }
    ATH_MSG_DEBUG( "Negative ME Track: " << neg_meTrack );
    m_Neg_ME_Pt = neg_meTrack->pt() / CLHEP::GeV;
    m_Neg_ME_Eta = neg_meTrack->eta();
    m_Neg_ME_Phi = neg_meTrack->phi();
    m_Neg_Quality = ( pair.second )->auxdata< int >( "MST_Quality" );  
    m_Neg_PtCone20 = ( pair.second )->auxdata< float >( "ptcone20" );
    m_Neg_Author = ( pair.second )->auxdata< uint16_t >( "author" );
    m_Neg_EnergyLossType = ( pair.second )->auxdata< uint8_t >( "energyLossType" );
    m_Neg_PrimarySector = ( pair.second )->auxdata< uint8_t >( "primarySector" );
    m_Neg_SecondarySector = ( pair.second )->auxdata< uint8_t >( "secondarySector" );

    TLorentzVector pos, neg;

    pos.SetPtEtaPhiM( m_Pos_Pt, m_Pos_Eta, m_Pos_Phi, MuonMass );
    neg.SetPtEtaPhiM( m_Neg_Pt, m_Neg_Eta, m_Neg_Phi, MuonMass );
    m_CB_Mass = ( pos + neg ).M(); 

    pos.SetPtEtaPhiM( m_Pos_ID_Pt, m_Pos_Eta, m_Pos_Phi, MuonMass );
    neg.SetPtEtaPhiM( m_Neg_ID_Pt, m_Neg_Eta, m_Neg_Phi, MuonMass );
    m_ID_Mass = ( pos + neg ).M(); 

    pos.SetPtEtaPhiM( m_Pos_ME_Pt, m_Pos_Eta, m_Pos_Phi, MuonMass );
    neg.SetPtEtaPhiM( m_Neg_ME_Pt, m_Neg_Eta, m_Neg_Phi, MuonMass );
    m_ME_Mass = ( pos + neg ).M(); 

    m_OutputTree->Fill();

  }

  return StatusCode::SUCCESS;

}
