/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//::: Framework includes
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

//::: Local includes
#include "MuonPtCalibNtupleMaker/CalibMuonsSelectorTool.h"

CalibMuonsSelectorTool::CalibMuonsSelectorTool( const std::string& type, const std::string& name, const IInterface* parent ) : ::AthAlgTool( type, name, parent )/*, m_MuonSelectorTool( "CP::MuonSelectionTool" )*/ {

  declareInterface< ICalibMuonsSelectorTool >( this );

  //::: Declaring properties
  declareProperty( "PtCut",          m_PtCut = 6. * CLHEP::GeV );
  declareProperty( "EtaCut",         m_EtaCut = 2.65 );
  declareProperty( "MinJpsiMassCut", m_MinJpsiMassCut = 2. * CLHEP::GeV );
  declareProperty( "MaxJpsiMassCut", m_MaxJpsiMassCut = 10. * CLHEP::GeV );
  declareProperty( "MinZMassCut",    m_MinZMassCut = 66. * CLHEP::GeV );
  declareProperty( "MaxZMassCut",    m_MaxZMassCut = 116. * CLHEP::GeV );

  declareProperty( "MuonSelectorTool", m_MuonSelectorTool ); 
  declareProperty( "GRLTool",          m_GRLTool ); 
}

StatusCode CalibMuonsSelectorTool::initialize() {

  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  m_MuonSelectorTool.setTypeAndName( "CP::MuonSelectionTool/MuonSelectionTool" ); 
  ATH_CHECK( m_MuonSelectorTool.retrieve() );

  m_GRLTool.setTypeAndName( "GoodRunsListSelectionTool/GRLTool" ); 
  ATH_CHECK( m_GRLTool.retrieve() );

  return StatusCode::SUCCESS;

}

StatusCode CalibMuonsSelectorTool::finalize() {

  ATH_MSG_INFO ( "Finalizing " << name() << "..." );
  return StatusCode::SUCCESS;

}

PairsVector CalibMuonsSelectorTool::GetMuons( const xAOD::MuonContainer* the_muons, TH1F* cut_flow ) {

  PairsVector SelectedMuons;

  //::: First Check if GRL is Passed
  cut_flow->Fill( 0.5 ); //::: Start
  const xAOD::EventInfo* evtInfo = 0;
  StatusCode sc = evtStore()->retrieve( evtInfo, "EventInfo" );
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Failed to retrieve EventInfo object" );
    return SelectedMuons;
  }
  bool pass_GRL = m_GRLTool->passRunLB( *evtInfo );
  ATH_MSG_DEBUG( "RunNumber: " << evtInfo->runNumber() << ", LumiBlock: " << evtInfo->lumiBlock() << ", Passes GRL: " << std::boolalpha << pass_GRL );
  if( pass_GRL ) cut_flow->Fill( 1.5 ); //::: Pass GRL 
  else return SelectedMuons;

  ATH_MSG_DEBUG( " ---> Number of muons in container: " << the_muons->size() );

  bool at_least_two_muons = false;
  bool in_mass_window = false;
  //::: First Loop Over Container
  for( auto the_first_muon = the_muons->begin(); the_first_muon != the_muons->end(); the_first_muon++ ) {

    //::: Quality Check
    if( ! IsGoodMuon( *the_first_muon ) ) continue;

    //::: Second Loop Over Container
    for( auto the_second_muon = the_first_muon + 1; the_second_muon != the_muons->end(); the_second_muon++ ) {

      //::: Quality Check
      if( ! IsGoodMuon( *the_second_muon ) ) continue;

      at_least_two_muons = true;
      ATH_MSG_DEBUG( " ------> Found two good muons " << *the_first_muon << " & " << *the_second_muon );

      ATH_MSG_DEBUG( " ------> Muon 1, pt: " << ( *the_first_muon )->pt() << ", eta: " << ( *the_first_muon )->eta() << ", phi: " << ( *the_first_muon )->phi() );
      ATH_MSG_DEBUG( " ------> Muon 2, pt: " << ( *the_second_muon )->pt() << ", eta: " << ( *the_second_muon )->eta() << ", phi: " << ( *the_second_muon )->phi() );
      ATH_MSG_DEBUG( " ------> Mass: " << ( ( *the_first_muon )->p4() + ( *the_second_muon )->p4() ).M() );

      //::: Mass Window Check
      if( ! AreInMassWindow( *the_first_muon, *the_second_muon ) ) continue;
      in_mass_window = true;

      //::: Charge Check
      //if( ! AreOppositelyCharged( *the_first_muon, *the_second_muon ) ) continue;

      //::: Decorate with quality
      ( *the_first_muon )->auxdecor< int >( "MST_Quality" ) = m_MuonSelectorTool->getQuality( **the_first_muon );
      ( *the_second_muon )->auxdecor< int >( "MST_Quality" ) = m_MuonSelectorTool->getQuality( **the_second_muon );

      //::: Filling
      if( ( *the_first_muon )->charge() > 0. ) {
        SelectedMuons.push_back( std::make_pair( *the_first_muon, *the_second_muon ) ); 
      }
      else {
        SelectedMuons.push_back( std::make_pair( *the_second_muon, *the_first_muon ) ); 
      }
    }
  }
  
  if( at_least_two_muons ) cut_flow->Fill( 2.5 ); //::: >= 2 muons
  if( in_mass_window ) cut_flow->Fill( 3.5 ); //::: mass window cut

  ATH_MSG_DEBUG( " ---> Number of pairs selected:     " << SelectedMuons.size() );

  return SelectedMuons; 
}

bool CalibMuonsSelectorTool::IsGoodMuon( const xAOD::Muon* the_muon ) {

  //::: Requiring Combined muons
  if( the_muon->muonType() != xAOD::Muon::MuonType::Combined ) return false;

  //::: Check if valid ElementLink to id Track
  if( the_muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle ) == NULL ) return false;

  //::: Check if valid ElementLink to ms Track
  if( the_muon->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle ) == NULL ) return false;
  
  //::: ID Quality Cuts with MuonSelectorTool
  //if( !m_MuonSelectorTool->passedIDCuts( *the_muon ) ) return false;

  //::: Kinematic Cuts
  if( the_muon->pt() < m_PtCut ) return false;
  if( fabs( the_muon->eta() ) > m_EtaCut ) return false;

  //::: If We Got This Far...
  return true;
}

bool CalibMuonsSelectorTool::AreInMassWindow( const xAOD::Muon* first_muon, const xAOD::Muon* second_muon ) {

  double invariant_mass = ( first_muon->p4() + second_muon->p4() ).M();

  //::: Jpsi Mass Window
  if( m_MinJpsiMassCut < invariant_mass && invariant_mass < m_MaxJpsiMassCut ) return true;

  //::: Z Mass Window
  if( m_MinZMassCut < invariant_mass && invariant_mass < m_MaxZMassCut ) return true;

  //::: If We Got This Far...
  return false;
}

bool CalibMuonsSelectorTool::AreOppositelyCharged( const xAOD::Muon* first_muon, const xAOD::Muon* second_muon ) {

  return ( first_muon->charge() * second_muon->charge() < 0 );

}
