/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CMMENERGYSUBBLOCK_H
#define TRIGT1CALOBYTESTREAM_CMMENERGYSUBBLOCK_H

#include <stdint.h>
#include <vector>

#include "CmmSubBlock.h"

namespace LVL1BS {

/** Sub-Block class for CMM-Energy data.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version 1.06d
 *
 *  @author Peter Faulkner
 */

class CmmEnergySubBlock : public CmmSubBlock {

 public:
   enum SourceId { REMOTE = 20, LOCAL = 23, TOTAL = 26, MAX_SOURCE_ID };

   CmmEnergySubBlock();
   ~CmmEnergySubBlock();

   /// Clear all data
   void clear();

   /// Return Ex subsum for given JEM or source ID
   unsigned int  ex(int slice, int source) const;
   /// Return Ey subsum for given JEM or source ID
   unsigned int  ey(int slice, int source) const;
   /// Return Et subsum for given JEM or source ID
   unsigned int  et(int slice, int source) const;
   /// Return Ex subsum error for given JEM or source ID
   int  exError(int slice, int source) const;
   /// Return Ey subsum error for given JEM or source ID
   int  eyError(int slice, int source) const;
   /// Return Et subsum error for given JEM or source ID
   int  etError(int slice, int source) const;
   /// Return Missing-ET Hits map
   unsigned int  missingEtHits(int slice) const;
   /// Return Sum-Et Hits map
   unsigned int  sumEtHits(int slice) const;
   /// Return Missing-ET-Sig Hits map
   unsigned int  missingEtSigHits(int slice) const;

   /// Store energy subsums and errors for given JEM or source ID
   void setSubsums(int slice, int source, unsigned int ex,
	           unsigned int ey, unsigned int et,
                   int exError, int eyError, int etError);
   /// Store Missing-ET Hits map
   void setMissingEtHits(int slice, unsigned int map);
   /// Store Sum-Et Hits map
   void setSumEtHits(int slice, unsigned int map);
   /// Store Missing-ET-Sig Hits map
   void setMissingEtSigHits(int slice, unsigned int map);

   /// Pack data
   bool pack();
   /// Unpack data
   bool unpack();

 private:
   /// Data word length
   static const int      s_wordLength      = 32;
   //  Energy subsums bit positions and masks
   static const int      s_exBit           = 0;
   static const int      s_eyBit           = 8;
   static const int      s_etBit           = 16;
   static const int      s_jemErrorBit     = 24;
   static const int      s_errorBit        = 15;
   static const int      s_etMissBit       = 16;
   static const int      s_etHitsBit       = 16;
   static const int      s_etMissSigBit    = 16;
   static const int      s_sourceIdBit     = 25;
   static const int      s_dataWordIdBit   = 30;
   static const int      s_dataWordId      = 0;
   static const int      s_maxSums         = 29;
   static const int      s_maxJems         = 16;
   static const uint32_t s_exMask          = 0xff;
   static const uint32_t s_eyMask          = 0xff;
   static const uint32_t s_etMask          = 0xff;
   static const uint32_t s_errorMask       = 0x1;
   static const uint32_t s_remoteErrorMask = 0x3;
   static const uint32_t s_etMissMask      = 0xff;
   static const uint32_t s_etHitsMask      = 0xff;
   static const uint32_t s_etHitsMaskV1    = 0xf;
   static const uint32_t s_etMissSigMask   = 0xff;
   static const uint32_t s_sumsMask        = 0x7fff;
   static const uint32_t s_sourceIdMask    = 0x1f;
   //  Neutral format
   static const int      s_jemSumBits        = 8;
   static const int      s_sumBits           = 15;
   static const int      s_bunchCrossingBits = 12;
   static const int      s_paddingBits       = 8;
   static const int      s_fifoOverflowPin   = 15;

   int  sourceId(uint32_t word)      const;
   int  index(int slice, int source) const;
   void resize();

   /// Pack neutral data
   bool packNeutral();
   /// Pack uncompressed data
   bool packUncompressed();
   /// Unpack neutral data
   bool unpackNeutral();
   /// Unpack uncompressed data
   bool unpackUncompressed();

   /// Energy subsums data
   std::vector<uint32_t> m_sumsData;

};

inline int CmmEnergySubBlock::sourceId(const uint32_t word) const
{
  return (word >> s_sourceIdBit) & s_sourceIdMask;
}

} // end namespace

#endif
