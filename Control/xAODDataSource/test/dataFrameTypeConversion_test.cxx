//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "helpers.h"
#include "xAODDataSource/MakeDataFrame.h"

// Framework include(s).
#include "xAODRootAccess/Init.h"
#include "CxxUtils/ubsan_suppress.h"
#include "AsgMessaging/MessageCheck.h"

// EDM include(s).
#include "xAODBase/IParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"

#include <TInterpreter.h>

// System include(s).
#include <iostream>

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

   // Use the electrons through their base class.
   auto df1 = df.Define( "el_pt_from_iparticle",
                         []( const xAOD::IParticleContainer& particles ) {
                            std::vector< float > result;
                            result.reserve( particles.size() );
                            for( const xAOD::IParticle* p : particles ) {
                               result.push_back( p->pt() );
                            }
                            return result;
                         }, { "Electrons" } );

   // Use the electrons through their concrete type.
   auto df2 = df.Define( "el_pt_from_electron",
                         []( const xAOD::ElectronContainer& electrons ) {
                            std::vector< float > result;
                            result.reserve( electrons.size() );
                            for( const xAOD::Electron* el : electrons ) {
                               result.push_back( el->pt() );
                            }
                            return result;
                         }, { "Electrons" } );

   // Make the code run.
   std::cout << "el_pt_from_iparticle.Mean() = "
             << df1.Mean( "el_pt_from_iparticle" ).GetValue() << std::endl;
   std::cout << "el_pt_from_electron.Mean() = "
             << df2.Mean( "el_pt_from_electron" ).GetValue() << std::endl;

   // Return gracefully.
   return 0;
}
