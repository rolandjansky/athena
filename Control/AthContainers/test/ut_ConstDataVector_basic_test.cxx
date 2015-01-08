/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_ConstDataVector_basic_test.cxx 626924 2014-11-07 15:00:54Z krasznaa $

// System include(s):
#include <cassert>

// Local include(s):
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"

/// Structure used in the test
struct DummyStruct {
   int m_dummy = 0; ///< Dummy variable
};

int main() {

   // Create a ConstDataVector:
   ConstDataVector< DataVector< DummyStruct > > cdv;
   cdv.push_back( new DummyStruct() );
   cdv.push_back( new DummyStruct() );

   // Some dummy test:
   assert( cdv.size() == 2 );

   // Try looping over it:
   for( const DummyStruct* d : cdv ) {
      assert( d->m_dummy == 0 );
   }
   ConstDataVector< DataVector< DummyStruct > >::const_iterator itr =
      cdv.begin();
   ConstDataVector< DataVector< DummyStruct > >::const_iterator end =
      cdv.end();
   for( ; itr != end; ++itr ) {
      assert( ( *itr )->m_dummy == 0 );
   }

   // Return gracefully:
   return 0;
}
