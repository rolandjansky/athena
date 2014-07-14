/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CMMCPSUBBLOCK_H
#define TRIGT1CALOBYTESTREAM_CMMCPSUBBLOCK_H

#include <stdint.h>
#include <vector>

#include "CmmSubBlock.h"

namespace LVL1BS {

/** Sub-Block class for CMM-CP data.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version 1.06d
 *
 *  @author Peter Faulkner
 */

class CmmCpSubBlock : public CmmSubBlock {

 public:
   //  Note - because CPMs are numbered 1-14 these are one more
   //  than the stored source ID and corresponding G-Link pin.
   enum SourceId { REMOTE_0 = 15, REMOTE_1 = 16, REMOTE_2 = 17,
                   LOCAL = 18, TOTAL = 19, MAX_SOURCE_ID };

   CmmCpSubBlock();
   ~CmmCpSubBlock();

   /// Clear all data
   void clear();

   /// Return hit counts for given CPM or source ID
   unsigned int hits(int slice, int source) const;
   /// Return hit error for given CPM or source ID
   int hitsError(int slice, int source)     const;

   /// Store hit counts for given CPM or source ID
   void setHits(int slice, int source, unsigned int hits, int error);

   /// Pack data
   bool pack();
   /// Unpack data
   bool unpack();

 private:
   /// Data word length
   static const int      s_wordLength      = 32;
   //  Jet hit counts bit positions and masks
   static const int      s_threshBit       = 0;
   static const int      s_threshErrorBit  = 24;
   static const int      s_sourceIdBit     = 25;
   static const int      s_dataWordIdBit   = 30;
   static const int      s_dataWordId      = 0;
   static const int      s_maxHits         = 19;
   static const uint32_t s_threshMask      = 0xffffff;
   static const uint32_t s_errorMask       = 0x1;
   static const uint32_t s_sourceIdMask    = 0x1f;
   //  Neutral format
   static const int      s_hitsBits          = 24;
   static const int      s_hitsErrorBits     = 1;
   static const int      s_bunchCrossingBits = 12;
   static const int      s_paddingBits       = 8;
   static const int      s_fifoOverflowPin   = 13;

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

   /// CPM hits and sums data
   std::vector<uint32_t> m_hitsData;

};

inline int CmmCpSubBlock::sourceId(const uint32_t word) const
{
  return (word >> s_sourceIdBit) & s_sourceIdMask;
}

} // end namespace

#endif
