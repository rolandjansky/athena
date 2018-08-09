/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_DATAERROR_H
#define TRIGT1CALO_DATAERROR_H

#include <string>

namespace LVL1 {

/** Error data.
 *
 *  Defines the error data bits unpacked from bytestream subdivided as
 *  follows:
 *
 *  Bits 0-2   CP/JEP specific error bits
 *  Bits 4-11  PP specific error bits
 *  Bits 4-8   CMX specific error bits
 *  Bits 16-29 Sub-status word error bits and failing BCN
 *
 *  Other bits are spare.  Uses an int for backwards compatibility
 *  reasons so probably bit 31 should not be used.
 *
 *  @author Peter Faulkner
 */

class DataError {

 public:

   enum ErrorBit { Overflow = 0, Parity = 1, LinkDown = 2,

                   ChannelDisabled = 4, MCMAbsent = 5, Timeout = 6,
                   ASICFull = 7, EventMismatch = 8, BunchMismatch = 9,
                   FIFOCorrupt = 10, PinParity = 11,

		   ParityMerge = 4, ParityPhase0 = 5, ParityPhase1 = 6,
		   ParityPhase2 = 7, ParityPhase3 = 8,

                   GLinkParity = 16, GLinkProtocol = 17, BCNMismatch = 18,
                   FIFOOverflow = 19, ModuleError = 20, GLinkDown = 22,
                   GLinkTimeout = 23, FailingBCN = 24,
		   
		   PPMErrorWord = 32, SubStatusWord = 33 };

   DataError();
   explicit DataError(int error);

   ~DataError();

   /// Set an error bit or data
   void set(ErrorBit bit, int value = 1);
   /// Return an error bit or data
   int  get(ErrorBit bit)       const;
   /// Return an error bit or data
   int  get(int bit)            const;
   /// Return the name of an error bit or data
   std::string bitName(int bit) const;
   /// Return the full error word
   int  error()                 const;

 private:

   /// Mask for Failing BCN
   static const int s_failingBcnMask = 0x3f;
   /// Mask for PPM error bits
   static const int s_ppmErrorMask   = 0xff;
   /// Mask for error bits from sub-status word
   static const int s_subStatusMask  = 0xff;
   /// Bit position of failing BCN in sub-status word
   static const int s_subStatusBcn   = 22;

   /// Error word
   int m_error;

};

inline int DataError::error() const
{
  return m_error;
}

} // end namespace

#endif
