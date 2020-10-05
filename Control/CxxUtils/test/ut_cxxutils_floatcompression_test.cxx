/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodcore_floatcompression_test.cxx 789425 2016-12-13 10:50:12Z krasznaa $
//
// Unit test for the CxxUtils::FloatCompressor class.
//

// System include(s):
#include <iostream>
#include <iomanip>

// Local include(s):
#include "CxxUtils/FloatCompressor.h"

/// Helper function printing the results of some compressions
void testCompression( const CxxUtils::FloatCompressor& fc, float value ) {

   std::cout << "Original value: " << std::dec << value << std::endl;
   CxxUtils::FloatCompressor::floatint_t compressed;
   compressed.fvalue = fc.reduceFloatPrecision( value );
   std::cout << "Compressed value: " << compressed.fvalue
             << " (hex: " << std::hex << std::setw( 8 ) << std::setfill( '0' )
             << compressed.ivalue << ")" << std::endl;
   return;
}

int main() {

   // Create a compressor leaving only 7 mantissa bits:
   const CxxUtils::FloatCompressor fc1( 7 );

   // Try to compress some numbers, and see what we get:
   testCompression( fc1, 1.2345 );
   testCompression( fc1, 3.1415 );

   // Now test a compressor leaving 16 mantissa bits:
   const CxxUtils::FloatCompressor fc2( 16 );

   // Try some numbers with this as well:
   testCompression( fc2, 2.7182 );
   testCompression( fc2, 0.5 );

   // Finally, test that when we use the maximal number of mantissa bits for
   // a float, the number doesn't get changed.
   const CxxUtils::FloatCompressor fc3( 23 );

   // Look at some numbers with this as well:
   testCompression( fc3, 2.3456 );
   testCompression( fc3, 0.1234 );

   return 0;
}
