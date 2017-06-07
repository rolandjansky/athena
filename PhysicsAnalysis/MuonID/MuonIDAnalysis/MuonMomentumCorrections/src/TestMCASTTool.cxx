/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

// Local include(s):
#include "TestMCASTTool.h"
#include <cmath>

namespace CP {

TestMCASTTool::TestMCASTTool( const std::string& name, ISvcLocator* svcLoc ) :
  AthAlgorithm( name, svcLoc ),
  m_Tool( "CP::MuonCalibrationAndSmearingTool/MuonCalibrationAndSmearingTool", this ),
  m_eta(-99.), m_phi(-99.), m_pt(-99.), m_ptcorr(-99.), m_ptdiff(-99.), m_ptdiffabs(-99.), m_ptid(-99.), m_ptms(-99.)
{

  declareProperty( "SGKey", m_sgKey = "Muons" );
  declareProperty( "MuonCalibrationAndSmearingTool", m_Tool );

  m_smearfile = NULL;
  m_smeartree = NULL;

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

  xAOD::MuonContainer* mymuons = new xAOD::MuonContainer;
  ATH_CHECK( evtStore()->record(mymuons,"CalibMuons") );
  xAOD::MuonAuxContainer* mymuonsaux = new xAOD::MuonAuxContainer;
  ATH_CHECK( evtStore()->record(mymuonsaux,"CalibMuonsAux.") );
  mymuons->setStore(mymuonsaux);


  for( ; mu_itr != mu_end; ++mu_itr ) {
    //---\\---// Simple preselection
    if( ( *mu_itr )->muonType() != xAOD::Muon::Combined ) continue;  
    //---\\---// Printing info
    ATH_MSG_INFO( "Analizing muon #" << mu_itr - muons->begin() );
    ATH_MSG_INFO( std::setw( 30 ) << "Selected muon: eta = " << std::setw( 8 ) << ( *mu_itr )->eta() << ", phi = " << std::setw( 8 ) << ( *mu_itr )->phi() << ", pt = " << std::setw( 8 ) << ( *mu_itr )->pt() );
    m_eta = ( *mu_itr )->eta();
    m_phi = ( *mu_itr )->phi();
    m_pt = ( *mu_itr )->pt();
    //---\\---// Calibrating muon
    ATH_MSG_DEBUG( "Calibrating muon" ); 
    xAOD::Muon* mu = 0;
    if( m_Tool->correctedCopy( **mu_itr, mu ) == CP::CorrectionCode::Error  ) {
      ATH_MSG_WARNING( "Failed to correct the muon!" );
      continue;
    }
    xAOD::Muon* mymuon = new xAOD::Muon();
    mymuon->makePrivateStore( **mu_itr );
    mymuons->push_back( mymuon );
    if( m_Tool->applyCorrection( *mymuon ) == CP::CorrectionCode::Error ) {
      ATH_MSG_WARNING( "Problem applying muon calibration" );
    }

    ATH_MSG_INFO( std::setw( 30 ) << "Calibrated muon: eta = " << std::setw( 8 ) << mu->eta() << ", phi = " << std::setw( 8 ) << mu->phi() << ", pt = " << std::setw( 8 ) << mu->pt() );
    ATH_MSG_INFO( "Calibration result: original pt = " << ( *mu_itr )->pt() << " / corrected pt = " << mu->pt() );
    ATH_MSG_INFO( "Calibration result: ID pt = " << mu->auxdata< float >( "InnerDetectorPt" ) );
    ATH_MSG_INFO( "Calibration result: ME pt = " << mu->auxdata< float >( "MuonSpectrometerPt" ) );
    //---\\---// Remove calibrated muon
    m_ptcorr = mu->pt();
    m_ptdiff = m_pt - m_ptcorr;
    m_ptdiffabs = std::abs( m_pt - m_ptcorr );
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
