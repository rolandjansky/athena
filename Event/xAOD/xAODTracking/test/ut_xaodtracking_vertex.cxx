/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
//
// Unit tests for xAOD::Vertex. Should be extended with more tests later on.
//

// System include(s):
#include <iostream>
#include <cmath>

// Local include(s):
#include "xAODTracking/Vertex.h"

/// Helper macro for evaluating logical tests
#define SIMPLE_ASSERT( EXP )                                                 \
   do {                                                                      \
      const bool result = EXP;                                               \
      if( ! result ) {                                                       \
         std::cout << __FILE__ << ":" << __LINE__                            \
                   << " Expression \"" << #EXP << "\" failed the evaluation" \
                   << std::endl;                                             \
         return 1;                                                           \
      }                                                                      \
   } while( 0 )

int main() {

   // Create the object to be tested:
   xAOD::Vertex v;
   v.makePrivateStore();

   // Make some very simple setter/getter checks:
   v.setFitQuality( 1.0, 5.0 );
   SIMPLE_ASSERT( std::abs( v.chiSquared() - 1.0 ) < 0.001 );
   SIMPLE_ASSERT( std::abs( v.numberDoF() - 5.0 ) < 0.001 );

   // Test the deep copying of the class:
   xAOD::Vertex copy;
   copy = v;
   SIMPLE_ASSERT( std::abs( copy.chiSquared() - 1.0 ) < 0.001 );
   SIMPLE_ASSERT( std::abs( copy.numberDoF() - 5.0 ) < 0.001 );

   // Return gracefully:
   return 0;
}
