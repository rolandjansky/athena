/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CMXJETSUBBLOCK_H
#define TRIGT1CALOBYTESTREAM_CMXJETSUBBLOCK_H

#include <stdint.h>
#include <vector>

#include "CmxSubBlock.h"

namespace LVL1BS {

/** Sub-Block class for CMX-Jet data post LS1.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version X.xxx                                           //<<== CHECK
 *
 *  @author Peter Faulkner
 */

class CmxJetSubBlock : public CmxSubBlock {

public:
  enum SourceId { REMOTE_MAIN = 0, LOCAL_MAIN = 1, TOTAL_MAIN = 2,
                  REMOTE_FORWARD = 4, LOCAL_FORWARD = 5, TOTAL_FORWARD = 6,
                  TOPO_CHECKSUM = 8, TOPO_OCCUPANCY_MAP = 9, TOPO_OCCUPANCY_COUNTS = 10,
                  MAX_SOURCE_ID = 11
                };

  CmxJetSubBlock();
  ~CmxJetSubBlock();

  /// Clear all data
  void clear();

  /// Return presence map for given JEM
  unsigned int presenceMap(int slice, int jem)       const;
  /// Return frame for given jem and tob
  int frame(int slice, int jem, int tob)             const;
  /// Return Local coordinate for given jem and tob
  int localCoord(int slice, int jem, int tob)        const;
  /// Return energy large window size for given jem and tob
  int energyLarge(int slice, int jem, int tob)       const;
  /// Return energy small window size for given jem and tob
  int energySmall(int slice, int jem, int tob)       const;
  /// Return error bit for given jem and tob
  int tobError(int slice, int jem, int tob)          const;
  /// Return parity bits for given JEM
  int parityBits(int slice, int jem)                 const;
  /// Return hit/topo counts for given source ID and HL flag
  unsigned int hits(int slice, int source, int flag) const;
  /// Return hit error for given source ID and HL flag
  int hitsError(int slice, int source, int flag)     const;

  /// Store presence map
  void setPresenceMap(int slice, int jem, unsigned int map);
  /// Store TOB (RoI) data for given JEM, frame, local coord
  void setTob(int slice, int jem, int frame, int loc,
              int energyLarge, int energySmall, int error);
  /// Store parity bits for neutral format
  void setParityBits(int slice, int jem, int parity);
  /// Store hit counts for given source ID and HL flag
  void setHits(int slice, int source, int flag, unsigned int hits, int error);

  /// Pack data
  bool pack();
  /// Unpack data
  bool unpack();

private:
  /// Data word length
  static const int      s_wordLength      = 32;
  //  TOB bit positions and masks
  static const int      s_tobEnergyLgBit   = 0;
  static const int      s_tobEnergySmBit   = 10;
  static const int      s_tobErrorBit      = 19;
  static const int      s_tobCoordBit      = 20;
  static const int      s_tobFrameBit      = 22;
  static const int      s_tobJemBit        = 25;
  static const int      s_tobWordId        = 0;
  static const uint32_t s_tobEnergyLgMask  = 0x3ff;
  static const uint32_t s_tobEnergySmMask  = 0x1ff;
  static const uint32_t s_tobErrorMask     = 0x1;
  static const uint32_t s_tobCoordMask     = 0x3;
  static const uint32_t s_tobFrameMask     = 0x7;
  static const uint32_t s_tobJemMask       = 0xf;
  //  Jet hit and topo counts bit positions and masks
  static const int      s_threshBit        = 0;
  static const int      s_threshErrorBit   = 16;
  static const int      s_threshHlFlagBit  = 19;
  static const int      s_topoHlFlagBit    = 24;
  static const int      s_hlFlagBit        = 19;
  static const int      s_sourceIdBit      = 25;
  static const int      s_dataWordIdBit    = 29;
  static const int      s_threshWordId     = 1;
  static const uint32_t s_threshMainMask   = 0x7fff;
  static const uint32_t s_threshFwdLMask   = 0xffff;
  static const uint32_t s_threshFwdHMask   = 0x3fff;
  static const uint32_t s_errorMask        = 0x7;    // Includes RoI overflow
  static const uint32_t s_topoCheckMask    = 0xffff;
  static const uint32_t s_topoMapMask      = 0xffff;
  static const uint32_t s_topoCountsMask   = 0xffffff;
  static const uint32_t s_hlFlagMask       = 0x1;
  static const uint32_t s_sourceIdMask     = 0xf;
  static const uint32_t s_dataWordIdMask   = 0x7;
  //  Neutral format
  static const int      s_presenceBits      = 8;
  static const int      s_coordBits         = 2;
  static const int      s_energyLgBits      = 10;
  //static const int      s_energySmBits[8]   = {3,6,5,4,7,2,9,0};
  static const int      s_energySmBits[8];
  static const int      s_threshMainBits    = 15;
  static const int      s_threshFwdLBits    = 16;
  static const int      s_threshFwdHBits    = 14;
  static const int      s_parityErrorBits   = 1;
  static const int      s_roiOverflowBits   = 1;
  static const int      s_paddingBits       = 31;
  static const int      s_bunchCrossingBits = 12;
  static const int      s_fifoOverflowBits  = 1;
  static const int      s_topoChecksumBits  = 16;
  static const int      s_topoMapBits       = 16;
  static const int      s_topoCountsBits    = 24;
  static const int      s_topoPaddingBits   = 3;
  static const int      s_glinkPins         = 20;
  static const int      s_modules           = 16;
  static const int      s_tobsPerModule     = 4;
  static const int      s_muxPhases         = 4;

  int dataWordId(uint32_t word) const;
  int sourceId(uint32_t word)   const;
  int jem(uint32_t word)        const;
  int hlFlag(uint32_t word)     const;
  unsigned int mapIndex(int slice, int jem)              const;
  unsigned int parIndex(int slice, int jem)              const;
  unsigned int tobIndex(int slice, int jem, int tob)     const;
  unsigned int hitIndex(int slice, int source, int flag) const;
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
  /// Parity data for neutral format
  std::vector<int> m_parityBits;
  /// JEM TOB count vector for unpacking
  std::vector<int> m_jemTobCount;

};

inline int CmxJetSubBlock::dataWordId(const uint32_t word) const
{
  return (word >> s_dataWordIdBit) & s_dataWordIdMask;
}

inline int CmxJetSubBlock::sourceId(const uint32_t word) const
{
  return (word >> s_sourceIdBit) & s_sourceIdMask;
}

inline int CmxJetSubBlock::jem(const uint32_t word) const
{
  return (word >> s_tobJemBit) & s_tobJemMask;
}

inline int CmxJetSubBlock::hlFlag(const uint32_t word) const
{
  uint8_t shift = (((word >> s_sourceIdBit) & s_sourceIdMask) < 7 ? s_threshHlFlagBit : s_topoHlFlagBit);
  return (word >> shift) & s_hlFlagMask;
}
// ============================================================================
} // end namespace
// ============================================================================
#endif
