//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "helpers.h"
#include "xAODDataSource/MakeDataFrame.h"

// xAOD include(s).
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODBase/IParticleContainer.h"

// ROOT include(s).
#include <ROOT/RDataFrame.hxx>
#include <TError.h>

// System include(s).
#include <iostream>

int main() {

   // Set up the runtime environment.
   CHECK( xAOD::Init() );

   // Create a data frame object.
   auto df = xAOD::MakeDataFrame( "${ASG_TEST_FILE_DATA}" );

   // Test its histogramming:
   auto elPt = df.Define( "ElectronsPt",
                          []( const xAOD::IParticleContainer& particles ) {
                             std::vector< float > result;
                             result.reserve( particles.size() );
                             for( const xAOD::IParticle* p : particles ) {
                                result.push_back( p->pt() );
                             }
                             return result;
                          },
                          { "Electrons" } );
   auto hist = elPt.Histo1D( "ElectronsPt" );
   std::cout << "ElectronsPt entries = " << hist->GetEntries() << std::endl;

   // Return gracefully.
   return 0;
}
