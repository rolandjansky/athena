// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FloatCompressor.h 789425 2016-12-13 10:50:12Z krasznaa $
#ifndef CXXUTILS_FLOATCOMPRESSOR_H
#define CXXUTILS_FLOATCOMPRESSOR_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

namespace CxxUtils {

   /// Class implementing a lossy float compression
   ///
   /// This is just a copy of the code implemented first in CaloUtils.
   /// Used to reduce the precision of select floating point quantities during
   /// the POOL conversion.
   ///
   /// @author Scott Snyder <snyder@bnl.gov>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 789425 $
   /// $Date: 2016-12-13 11:50:12 +0100 (Tue, 13 Dec 2016) $
   ///
   class FloatCompressor {

   public:
      /// Constructor with the number of mantissa bits to retain
      FloatCompressor( unsigned int mantissaBits = 7 );

      /// Function returning a reduced precision float value
      float reduceFloatPrecision( float value ) const;

      /// Type used in the compression
      union floatint_t {
         float fvalue;
         uint32_t ivalue;
      };

   private:
      /// Number of mantissa bits to keep
      unsigned int m_mantissaBits;
      /// Bitmask for zeroing out the non-interesting bits
      uint32_t m_mantissaBitmask;

      /// @name Magic numbers
      /// @{

      // Half of the LSB-value after cutting the lower 32 - Ntotal bits
      uint32_t m_rounding;
      /// Largest possible positive 32bit float minus the rounding
      uint32_t m_vmax;

      /// @}

   }; // class FloatCompressor

} // namespace CxxUtils

#endif // CXXUTILS_FLOATCOMPRESSOR_H
