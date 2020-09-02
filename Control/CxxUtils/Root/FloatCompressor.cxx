/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FloatCompressor.cxx 789425 2016-12-13 10:50:12Z krasznaa $

// System include(s):
#include <cmath>

// Local include(s):
#include "CxxUtils/FloatCompressor.h"

namespace CxxUtils {

   /// Total number of total mantissa bits
   static const unsigned int NMANTISSA = 23;

   FloatCompressor::FloatCompressor( unsigned int mantissaBits )
      : m_mantissaBits( mantissaBits ), m_mantissaBitmask( 0 ) {

      // IEEE754 single-precision float
      // SEEE EEEE EMMM MMMM MMMM MMMM MMMM MMMM
      //   F   F     8   0    0    0    7    F  

      // Definition:
      //
      // Assume that we'd like to keep only 7 bits in the mantissa
      // In this case the memory layout of the bits will be:
      //
      // Sign |  Exp (8 bits) | Frac (23 bits)
      // S       EEEEEEEE       FFFFFFLRTTTTTTTTTTTTTTT
      //
      // where
      //
      // S : Sign bit
      // E : Exponent bits
      // F : Fraction bits
      // L : Least significant bit (lsb) for 7 bits mantissa precision
      // R : Rounding bit
      // T : Sticky bits (i.e any bit after lsb + 1)
      //
      // In the current implementation there are essentially 4 cases:
      //
      // Case 1: L = 0 and R = 0
      // In this case there'll be rounding down
      //
      // Case 2: L = 1 and R = 0
      // In this case there'll be rounding down
      //
      // Case 3: L = 0 and R = 1
      // In this case there'll be rounding up
      //
      // Note: This scenario can be different than bfloat16 implementaion
      //       of TensorFlow, where they round down if all the Ts are zero.
      //       Otherwise, they also round up.
      //
      // Case 4: L = 1 and R = 1
      // In this case there'll be rounding up
      //
      // In all cases, we do an extra check to avoid overflow.
      //
      // From a technical point of view, the rounding is computed
      // to be the half of the lsb(=1) and added to the original value
      // as long as the new value doesn't overflow. Then the
      // undesired bits are masked. We never go below 5 bits in the
      // mantissa.

      // Adjust the received bit number to some reasonable value:
      if( m_mantissaBits < 5 ) {
         m_mantissaBits = 5;
      }
      if( m_mantissaBits > NMANTISSA ) {
         m_mantissaBits = NMANTISSA;
      }

      // Fill up the lower N bits:
      for( unsigned int i = 0; i < ( NMANTISSA - m_mantissaBits ); ++i ) {
         m_mantissaBitmask |= ( 0x1 << i );
      }
      // And now negate it to get the correct mask:
      m_mantissaBitmask = ~m_mantissaBitmask;

      // Set the Magic numbers
      if (m_mantissaBits == NMANTISSA) {
        m_rounding = 0;
      }
      else {
        m_rounding = 0x1 << ( 32 - (1 + 8 + m_mantissaBits) - 1 );
      }
      // The part below is taken from AthenaPoolCnvSvc/Compressor
      // and would work the same as long as the user doesn't
      // compress lower than 3 mantissa bits, which is not allowed
      // in any case.
      m_vmax = 0x7f7 << 20;
      m_vmax |= 0x000fffff xor (m_rounding);
   }

   float FloatCompressor::reduceFloatPrecision( float value ) const {

      // Check if any compression is to be made:
      if( m_mantissaBits == NMANTISSA ) {
         return value;
      }

      // Check for NaN, etc:
      if( ! std::isfinite( value ) ) {
         return value;
      }

      // Create the helper object:
      floatint_t fi;
      fi.fvalue = value;

      //safety-check if value (omitting the sign-bit) is lower than vmax
      //(avoid overflow)
      if( ( fi.ivalue & 0x7fffffff ) < m_vmax ) {
         fi.ivalue += m_rounding;
      }

      // Do the compression:
      fi.ivalue &= m_mantissaBitmask;
      return fi.fvalue;
   }

} // namespace CxxUtils
