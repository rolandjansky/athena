//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "EventLoopTest/UnitTestAlg3.h"

namespace EL {

   UnitTestAlg3::UnitTestAlg3( const std::string& name, ISvcLocator* svcLoc )
      : AnaAlgorithm( name, svcLoc ) {

      // Declare the algorithm's properties.
      declareProperty( "LeakBytes", m_leakBytes = 0,
                       "Number of bytes to leak per event" );
   }

   ::StatusCode UnitTestAlg3::execute() {

      // Allocate the requested amount of memory.
      if( m_leakBytes > 0 ) {
         char* lostPointer = new char[ m_leakBytes ];
         for( int i = 0; i < m_leakBytes; ++i ) {
            *lostPointer = 0;
            ++lostPointer;
         }
      }

      // Return gracefully.
      return ::StatusCode::SUCCESS;
   }

} // namespace EL
