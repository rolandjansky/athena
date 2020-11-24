//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "helpers.h"
#include "xAODDataSource/MakeDataFrame.h"

// Framework include(s).
#include "xAODRootAccess/Init.h"
#include "AsgMessaging/MessageCheck.h"

// EDM include(s).
#include "AthContainers/ConstDataVector.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "CxxUtils/ubsan_suppress.h"

#include <TInterpreter.h>

int main() {

   ANA_CHECK_SET_TYPE (int);
   using namespace asg::msgUserCode;

   // Suppress ubsan warning.
   CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });

   // Set up the runtime environment.
   ROOT::EnableImplicitMT();
   ANA_CHECK( xAOD::Init() );

   // Create a data frame object.
   auto df = xAOD::MakeDataFrame( "${ASG_TEST_FILE_DATA}" );

   // Define new variables using element links.
   auto variables = df
      .Define( "HighPtElectrons",
               []( const xAOD::ElectronContainer& electrons ) {
                  ConstDataVector< xAOD::ElectronContainer >
                     result( SG::VIEW_ELEMENTS );
                  for( const xAOD::Electron* el : electrons ) {
                     if( el->pt() > 10000.0 ) {
                        result.push_back( el );
                     }
                  }
                  return xAOD::ElectronContainer( *( result.asDataVector() ) );
               }, { "Electrons" } )
      .Define( "el_trk_pt_diff",
               []( const xAOD::ElectronContainer& electrons ) {
                  std::vector< float > result;
                  result.reserve( electrons.size() );
                  for( const xAOD::Electron* el : electrons ) {
                     result.push_back( el->pt() - el->trackParticle()->pt() );
                  }
                  return result;
               }, { "HighPtElectrons" } )
      .Define( "HighPtMuons",
               []( const xAOD::MuonContainer& muons ) {
                  ConstDataVector< xAOD::MuonContainer >
                     result( SG::VIEW_ELEMENTS );
                  for( const xAOD::Muon* mu : muons ) {
                     if( ( mu->pt() > 10000.0 ) &&
                         ( mu->muonType() == xAOD::Muon::Combined ) ) {
                        result.push_back( mu );
                     }
                  }
                  return xAOD::MuonContainer( *( result.asDataVector() ) );
               }, { "Muons" } )
      .Define( "mu_trk_pt_diff",
               []( const xAOD::MuonContainer& muons ) {
                  std::vector< float > result;
                  result.reserve( muons.size() );
                  for( const xAOD::Muon* mu : muons ) {
                     result.push_back( mu->pt() -
                                       mu->trackParticle( xAOD::Muon::InnerDetectorTrackParticle )->pt() );
                  }
                  return result;
               }, { "HighPtMuons" } );

   // Create histograms from these variables.
   auto el_hist = variables.Histo1D( { "el_hist",
                                       "Electron p_{T} - Track p_{T};"
                                       "p_{T} diff. [MeV];Entries",
                                       128, -100000.0, 100000.0 },
                                     "el_trk_pt_diff" );
   auto mu_hist = variables.Histo1D( { "mu_hist",
                                       "Muon p_{T} - Track p_{T};"
                                       "p_{T} diff. [MeV];Entries",
                                       128, -10000.0, 10000.0 },
                                     "mu_trk_pt_diff" );

   // Print some basic properties of the created histograms.
   std::cout << "el_hist->GetMaximumBin() = " << el_hist->GetMaximumBin()
             << std::endl;
   std::cout << "mu_hist->GetMaximumBin() = " << mu_hist->GetMaximumBin()
             << std::endl;

   // Return gracefully.
   return 0;
}
