//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "helpers.h"
#include "xAODDataSource/MakeDataFrame.h"

// xAOD include(s).
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODBase/IParticleContainer.h"
#include "CxxUtils/ubsan_suppress.h"
#include "AsgMessaging/MessageCheck.h"

// ROOT include(s).
#include <ROOT/RDataFrame.hxx>
#include <TError.h>
#include <TInterpreter.h>

// System include(s).
#include <iostream>

int main() {

   ANA_CHECK_SET_TYPE (int);
   using namespace asg::msgUserCode;

   // Suppress ubsan warning.
   CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });

   // Set up the runtime environment.
   ANA_CHECK( xAOD::Init() );

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
