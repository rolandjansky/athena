/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CMMJETSUBBLOCK_H
#define TRIGT1CALOBYTESTREAM_CMMJETSUBBLOCK_H

#include <stdint.h>
#include <vector>

#include "CmmSubBlock.h"

namespace LVL1BS {

/** Sub-Block class for CMM-Jet data.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version 1.06d
 *
 *  @author Peter Faulkner
 */

class CmmJetSubBlock : public CmmSubBlock {

 public:
   enum SourceId { REMOTE_MAIN = 16, LOCAL_MAIN = 17, TOTAL_MAIN = 18,
                 REMOTE_FORWARD = 20, LOCAL_FORWARD = 21, TOTAL_FORWARD = 22,
		 MAX_SOURCE_ID };

   CmmJetSubBlock();
   ~CmmJetSubBlock();

   /// Clear all data
   void clear();

   /// Return jet hit counts for given jem or source ID
   unsigned int jetHits(int slice, int source) const;
   /// Return jet hit error for given jem or source ID
   int jetHitsError(int slice, int source)     const;
   /// Return jet ET map
   unsigned int jetEtMap(int slice)            const;

   /// Store jet hit counts and error for given jem or source ID
   void setJetHits(int slice, int source, unsigned int hits,
	                                           int error);
   /// Store jet ET map
   void setJetEtMap(int slice, unsigned int map);

   /// Pack data
   bool pack();
   /// Unpack data
   bool unpack();

 private:
   /// Data word length
   static const int      s_wordLength      = 32;
   //  Jet hit counts bit positions and masks
   static const int      s_threshBit       = 0;
   static const int      s_fwdErrorBit     = 16;
   static const int      s_etMapBit        = 17;
   static const int      s_threshErrorBit  = 24;
   static const int      s_sourceIdBit     = 25;
   static const int      s_dataWordIdBit   = 30;
   static const int      s_dataWordId      = 0;
   static const int      s_maxHits         = 23;
   static const uint32_t s_threshMask      = 0xffffff;
   static const uint32_t s_fwdMask         = 0xffff;
   static const uint32_t s_etMapMask       = 0xf;
   static const uint32_t s_errorMask       = 0x1;
   static const uint32_t s_sourceIdMask    = 0x1f;
   //  Neutral format
   static const int      s_jetHitsBits       = 24;
   static const int      s_jetHitsErrorBits  = 1;
   static const int      s_fwdHitsBits       = 16;
   static const int      s_bunchCrossingBits = 12;
   static const int      s_paddingBits       = 8;
   static const int      s_rightBit          = 8;
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

   /// JEM hits and sums data
   std::vector<uint32_t> m_hitsData;

};

inline int CmmJetSubBlock::sourceId(const uint32_t word) const
{
  return (word >> s_sourceIdBit) & s_sourceIdMask;
}

} // end namespace

#endif
