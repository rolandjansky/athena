// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCORE_TOOLS_FLOATCOMPRESSOR_H
#define XAODCORE_TOOLS_FLOATCOMPRESSOR_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

namespace xAOD {

   /// Class implementing a lossy float compression
   ///
   /// This is just a copy of the code implemented first in CaloUtils.
   /// Used to reduce the precision of select floating point quantities during
   /// the POOL conversion.
   ///
   /// @author Scott Snyder <snyder@bnl.gov>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
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

      /// Half of the LSB-value after cutting the lower 16 bits
      static const uint32_t m_rounding=0x00008000;
      /// Largest possible positive 32bit float minus the rounding
      static const uint32_t m_vmax=0x7f7f7fff;

      /// @}

   }; // class FloatCompressor

} // namespace xAOD

#endif // XAODCORE_TOOLS_FLOATCOMPRESSOR_H
