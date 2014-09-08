/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "TestMCASTTool.h"

namespace CP {

TestMCASTTool::TestMCASTTool( const std::string& name, ISvcLocator* svcLoc ) :
  AthAlgorithm( name, svcLoc ),
  m_Tool( "CP::MuonCalibrationAndSmearingTool/MuonCalibrationAndSmearingTool", this ) {
  declareProperty( "SGKey", m_sgKey = "Muons" );
  declareProperty( "MuonCalibrationAndSmearingTool", m_Tool );
}

StatusCode TestMCASTTool::initialize() {
  ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
  ATH_MSG_DEBUG( "SGKey = " << m_sgKey );
  ATH_MSG_DEBUG( "MuonCalibrationAndSmearingTool   = " << m_Tool );
  ATH_CHECK( m_Tool.retrieve() );
  m_smearfile = new TFile( "smearingtrees.root","RECREATE","Smearing and non-Smearing of Muons" );
  m_smeartree = new TTree( "SmearTree", "This Tree contains the information of the muon after smearing effects" );
  m_smeartree->Branch( "eta",&m_eta,"eta/F" );
  m_smeartree->Branch( "phi",&m_phi,"phi/F" );
  m_smeartree->Branch( "pt",&m_pt,"pt/F" );
  m_smeartree->Branch( "ptid",&m_ptid,"ptid/F" );
  m_smeartree->Branch( "ptms",&m_ptms,"ptms/F" );
  m_smeartree->Branch( "ptcorr",&m_ptcorr,"ptcorr/F" );
  m_smeartree->Branch( "ptdiff",&m_ptdiff,"ptdiff/F" );
  m_smeartree->Branch( "ptdiffabs",&m_ptdiffabs,"ptdiffabs/F" );
  return StatusCode::SUCCESS;
}

StatusCode TestMCASTTool::execute() {
  //---\\---// Retrieving muons from container
  const xAOD::MuonContainer* muons = 0;
  ATH_CHECK( evtStore()->retrieve( muons, m_sgKey ) );
  ATH_MSG_INFO( "Number of muons: " << muons->size() );
  //---\\---// Looping over muons
  xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
  xAOD::MuonContainer::const_iterator mu_end = muons->end();
  for( ; mu_itr != mu_end; ++mu_itr ) {
    //---\\---// Simplified muon selection: eta, pt and author
    if( fabs( ( *mu_itr )->eta() ) > 2.5 ) continue;
    if( ( *mu_itr )->pt() < 6000 ) continue;
    if( ( *mu_itr )->author() != 12 ) continue;
    //---\\---// Printing info
    ATH_MSG_INFO( "Analizing muon #" << mu_itr - muons->begin() );
    ATH_MSG_INFO( std::setw( 30 ) << "Selected muon: eta = " << std::setw( 8 ) << ( *mu_itr )->eta() << ", phi = " << std::setw( 8 ) << ( *mu_itr )->phi() << ", pt = " << std::setw( 8 ) << ( *mu_itr )->pt() );
    const ElementLink< xAOD::TrackParticleContainer >& id_track = ( *mu_itr )->inDetTrackParticleLink();
    const ElementLink< xAOD::TrackParticleContainer >& ms_track = ( *mu_itr )->muonSpectrometerTrackParticleLink();
    if( id_track ) {
      ATH_MSG_INFO( std::setw( 30 ) << "ID track: eta = " << std::setw( 8 ) << ( *id_track )->eta() << ", phi = " << std::setw( 8 ) << ( *id_track )->phi() << ", pt = " << std::setw( 8 ) << ( *id_track )->pt() );
      m_ptid=( *id_track )->pt();
    }
    else {
      m_ptid=0;
    }
    if( ms_track ) {
      ATH_MSG_INFO( std::setw( 30 ) << "MS track: eta = " << std::setw( 8 ) << ( *ms_track )->eta() << ", phi = " << std::setw( 8 ) << ( *ms_track )->phi() << ", pt = " << std::setw( 8 ) << ( *ms_track )->pt() );
      m_ptms=( *ms_track )->pt();
    }
    else {
      m_ptms=0;
    }
    m_eta=( *mu_itr )->eta();
    m_phi=( *mu_itr )->phi();
    m_pt=( *mu_itr )->pt();
    //---\\---// Calibrating muon
    xAOD::Muon* mu = 0;
    if( ! m_Tool->correctedCopy( **mu_itr, mu ) ) {
      ATH_MSG_WARNING( "Failed to correct the muon!" );
      continue;
    }
    ATH_MSG_INFO( std::setw( 30 ) << "Calibrated muon: eta = " << std::setw( 8 ) << mu->eta() << ", phi = " << std::setw( 8 ) << mu->phi() << ", pt = " << std::setw( 8 ) << mu->pt() );
    //ATH_MSG_INFO( "Calibration result: original pt = " << ( *mu_itr )->pt() << " / corrected pt = " << mu->pt() );
    //---\\---// Remove calibrated muon
    m_ptcorr=mu->pt();
    m_ptdiff=( m_pt-m_ptcorr );
    m_ptdiffabs=abs( ( m_pt-m_ptcorr ) );
    m_smeartree->Fill();
    delete mu;
  }
  return StatusCode::SUCCESS;
}

StatusCode TestMCASTTool::finalize() {
  m_smearfile->Write();
  return StatusCode::SUCCESS;
}

} // namespace CP
