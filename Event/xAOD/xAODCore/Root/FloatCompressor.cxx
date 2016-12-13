/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FloatCompressor.cxx 789425 2016-12-13 10:50:12Z krasznaa $

// System include(s):
#include <cmath>

// Local include(s):
#include "xAODCore/tools/FloatCompressor.h"

namespace xAOD {

   /// Total number of total mantissa bits
   static const unsigned int NMANTISSA = 23;

   FloatCompressor::FloatCompressor( unsigned int mantissaBits )
      : m_mantissaBits( mantissaBits ), m_mantissaBitmask( 0 ) {

      // IEEE754 single-precision float
      // SEEE EEEE EMMM MMMM MMMM MMMM MMMM MMMM
      //   F   F     8   0    0    0    7    F  

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
      if( ( fi.ivalue & 0x7ffffff ) < m_vmax ) {
         fi.ivalue += m_rounding;
      }

      // Do the compression:
      fi.ivalue &= m_mantissaBitmask;
      return fi.fvalue;
   }

} // namespace xAOD
