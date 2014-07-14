/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CMXCPSUBBLOCK_H
#define TRIGT1CALOBYTESTREAM_CMXCPSUBBLOCK_H

#include <stdint.h>
#include <vector>

#include "CmxSubBlock.h"

namespace LVL1BS {

/** Sub-Block class for CMX-CP data post LS1.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version X.xxx                                                    <<== CHECK
 *
 *  @author Peter Faulkner
 */

class CmxCpSubBlock : public CmxSubBlock {

 public:

   /// Sources of threshold sums
   enum SourceId { REMOTE_0, REMOTE_1, REMOTE_2, LOCAL, TOTAL,
                   TOPO_CHECKSUM, TOPO_OCCUPANCY_MAP, TOPO_OCCUPANCY_COUNTS,
		   MAX_SOURCE_ID };

   CmxCpSubBlock();
   ~CmxCpSubBlock();

   /// Clear all data
   void clear();

   /// Return presence map for given CPM
   unsigned int presenceMap(int slice, int cpm)       const;
   /// Return chip for given cpm and tob
   int chip(int slice, int cpm, int tob)              const;
   /// Return Local coordinate for given cpm and tob
   int localCoord(int slice, int cpm, int tob)        const;
   /// Return isolation for given cpm and tob
   int isolation(int slice, int cpm, int tob)         const;
   /// Return energy for given cpm and tob
   int energy(int slice, int cpm, int tob)            const;
   /// Return error bits for given cpm and tob
   int tobError(int slice, int cpm, int tob)          const;
   /// Return hit/topo counts for given source ID and HL flag
   unsigned int hits(int slice, int source, int flag) const;
   /// Return hit error for given source ID and HL flag
   int hitsError(int slice, int source, int flag)     const;
   /// Return RoI overflow for given source ID
   int roiOverflow(int slice, int source)             const;

   /// Store presence map
   void setPresenceMap(int slice, int cpm, unsigned int map);
   /// Store TOB (RoI) data for given CPM, chip, local coord
   void setTob(int slice, int cpm, int chip, int loc,            //  NB chip and loc 4 and 2 bits here, as in diagram
               int energy, int isol, int error);
   /// Store hit counts for given source ID and HL flag
   void setHits(int slice, int source, int flag, unsigned int hits, int error);
   /// Store RoI overflow for given source ID
   void setRoiOverflow(int slice, int source, int overflow);

   /// Pack data
   bool pack();
   /// Unpack data
   bool unpack();

 private:
   /// Data word length
   static const int      s_wordLength       = 32;
   //  TOB bit positions and masks
   static const int      s_tobEnergyBit     = 0;
   static const int      s_tobIsolationBit  = 8;
   static const int      s_tobErrorBit      = 13;
   static const int      s_tobOverflowBit   = 18;
   static const int      s_tobCoordBit      = 19;
   static const int      s_tobChipBit       = 21;
   static const int      s_tobCpmBit        = 25;
   static const int      s_tobWordId        = 0;
   static const uint32_t s_tobEnergyMask    = 0xff;
   static const uint32_t s_tobIsolationMask = 0x1f;
   static const uint32_t s_tobErrorMask     = 0x3f;  // includes RoI overflow
   static const uint32_t s_tobCoordMask     = 0x3;
   static const uint32_t s_tobChipMask      = 0xf;
   static const uint32_t s_tobCpmMask       = 0xf;
   //  EM/Tau hit and topo counts bit positions and masks
   static const int      s_threshBit        = 0;
   static const int      s_threshErrorBit   = 24;
   static const int      s_hlFlagBit        = 25;
   static const int      s_sourceIdBit      = 26;
   static const int      s_dataWordIdBit    = 29;
   static const int      s_threshWordId     = 1;
   static const uint32_t s_threshMask       = 0xffffff;
   static const uint32_t s_errorMask        = 0x1;
   static const uint32_t s_hlFlagMask       = 0x1;
   static const uint32_t s_sourceIdMask     = 0x7;
   static const uint32_t s_dataWordIdMask   = 0x7;
   //  Neutral format
   static const int      s_presenceBits      = 16;
   static const int      s_coordBits         = 2;
   static const int      s_isolationBits     = 5;
   static const int      s_parityErrorBits   = 1;
   static const int      s_parityErrorMask   = 0x1;
   static const int      s_energyBits        = 8;
   static const int      s_hitsBits          = 24;
   static const int      s_hitsErrorBits     = 1;
   static const int      s_roiOverflowBits   = 1;
   static const int      s_paddingBits       = 45;
   static const int      s_bunchCrossingBits = 12;
   static const int      s_fifoOverflowBits  = 1;
   static const int      s_topoChecksumBits  = 16;
   static const int      s_topoMapBits       = 14;
   static const int      s_topoCountsBits    = 21;
   static const int      s_topoPaddingBits   = 11;
   static const int      s_glinkPins         = 20;
   static const int      s_modules           = 14;
   static const int      s_tobsPerModule     = 5;
   static const int      s_muxPhases         = 4;

   int dataWordId(uint32_t word) const;
   int sourceId(uint32_t word)   const;
   int cpm(uint32_t word)        const;
   int hlFlag(uint32_t word)     const;
   unsigned int mapIndex(int slice, int cpm)              const;
   unsigned int tobIndex(int slice, int cpm, int tob)     const;
   unsigned int hitIndex(int slice, int source, int flag) const;
   unsigned int ovfIndex(int slice, int source)           const;
   void resize();

   /// Pack neutral data
   bool packNeutral();
   /// Pack uncompressed data
   bool packUncompressed();
   /// Unpack neutral data
   bool unpackNeutral();
   /// Unpack uncompressed data
   bool unpackUncompressed();

   /// TOB data
   std::vector<uint32_t> m_tobData;
   /// Hits and topo data
   std::vector<uint32_t> m_hitsData;
   /// Presence maps
   std::vector<unsigned int> m_presenceMaps;
   /// CPM TOB count vector for unpacking
   std::vector<int> m_cpmTobCount;
   /// RoI overflows for neutral data
   std::vector<int> m_overflow;

};

inline int CmxCpSubBlock::dataWordId(const uint32_t word) const
{
  return (word >> s_dataWordIdBit) & s_dataWordIdMask;
}

inline int CmxCpSubBlock::sourceId(const uint32_t word) const
{
  return (word >> s_sourceIdBit) & s_sourceIdMask;
}

inline int CmxCpSubBlock::cpm(const uint32_t word) const
{
  return (word >> s_tobCpmBit) & s_tobCpmMask;
}

inline int CmxCpSubBlock::hlFlag(const uint32_t word) const
{
  return (word >> s_hlFlagBit) & s_hlFlagMask;
}

} // end namespace

#endif
