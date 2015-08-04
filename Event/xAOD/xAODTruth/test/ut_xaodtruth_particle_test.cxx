/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodtruth_particle_test.cxx 687313 2015-08-04 12:07:38Z krasznaa $

// System include(s):
#include <iostream>
#include <cmath>

// Local include(s):
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

/// Helper macro to test assertions
#define SIMPLE_ASSERT( EXP )                                                \
   do {                                                                     \
      if( ! ( EXP ) ) {                                                     \
         std::cout << "Error evaluating: " << #EXP << std::endl;            \
         return 1;                                                          \
      }                                                                     \
   } while( 0 )

int main() {

   // Create a container to test:
   xAOD::TruthParticleContainer c;
   xAOD::TruthParticleAuxContainer aux;
   c.setStore( &aux );

   // Construct a particle that we can test. The values are chosen on purpose
   // to not be possible to represent in exactly the same way at single and
   // double precision.
   static const double PX = std::sqrt( ( double ) 1.2345 );
   static const double PY = std::sqrt( ( double ) 2.3456 );
   static const double PZ = std::sqrt( ( double ) 3.4567 );
   static const double E  = std::sqrt( ( double ) 4.5678 );
   static const double M  = std::sqrt( ( double ) 5.6789 );
   xAOD::TruthParticle* p = new xAOD::TruthParticle();
   c.push_back( p );
   p->setPx( PX );
   p->setPy( PY );
   p->setPz( PZ );
   p->setE( E );
   p->setM( M );

   // The precisions that we want:
   static const double FLOAT_PRECISION  = 1e-5;
   static const double DOUBLE_PRECISION = 1e-10;

   // Start with the most basic checks:
   SIMPLE_ASSERT( std::abs( p->px() - PX ) < FLOAT_PRECISION );
   SIMPLE_ASSERT( std::abs( p->py() - PY ) < FLOAT_PRECISION );
   SIMPLE_ASSERT( std::abs( p->pz() - PZ ) < FLOAT_PRECISION );
   SIMPLE_ASSERT( std::abs( p->e() - E ) < FLOAT_PRECISION );
   SIMPLE_ASSERT( std::abs( p->m() - M ) < FLOAT_PRECISION );

   // Now something more tricky:
   SIMPLE_ASSERT( std::abs( p->pt() - p->p4().Pt() ) < DOUBLE_PRECISION );
   SIMPLE_ASSERT( std::abs( p->eta() - p->p4().Eta() ) < DOUBLE_PRECISION );
   SIMPLE_ASSERT( std::abs( p->phi() - p->p4().Phi() ) < DOUBLE_PRECISION );
   SIMPLE_ASSERT( std::abs( p->e() - p->p4().E() ) < DOUBLE_PRECISION );

   // And finally something not completely obvious:
   SIMPLE_ASSERT( std::abs( p->m() - p->p4().M() ) > 0.1 );

   // Let the user know what happened:
   std::cout << "All tests succeeded" << std::endl;

   // Return gracefully:
   return 0;
}
