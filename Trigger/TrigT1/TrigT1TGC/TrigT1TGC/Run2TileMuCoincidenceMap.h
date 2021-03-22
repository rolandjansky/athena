/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_Run2TileMuCoincidenceMap_h
#define TrigT1TGC_Run2TileMuCoincidenceMap_h

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

#include <map>

namespace LVL1TGC {

/**
 * @brief This class stores the map for Tile-Muon coincidence of the Run-2 L1Muon Endcap trigger.
 * The contents are read from the Condition database, and are kept in the TGCTriggerData object.
 * When the coincidence map is read from the text file, the contents is kept in member variables
 * of this class.
 */

/** Contents of Run-2 Tile-CW LUT
 * =============================
 * consists of three look-up tables
 * a) Applied pT in SSC (m_flagpt)
 *   std::unordered_map<ADDR, PTMASK>
 *  where
 *   ADDR   | 12 bits | uint16_t | side, trigger sector, and SSC
 *   PTMASK |  3 bits | uint8_t  | bit mask for applied pT for SSC
 *
 * b)Applied RoI in SSC (m_flagroi)
 *   std::unordered_map<ADDR, ROIMASK>
 *  where
 *   ADDR    | 12 bits | uint16_t | side, trigger sector, and SSC
 *   ROIMASK |  8 bits | uint8_t  | bit mask for applied RoI in SSC
 *
 * c) Trigger bit for each input (m_trigbit)
 *   std::unordered_map<ADDR, TRIGBIT>
 *  where TRIGBIT is:
 *  | Module D (Tile=3) || Module C (Tile=2) || Module B (Tile=1) || Module A (Tile=0) |
 *  | 15 | 14 | 13 | 12 || 11 | 10 |  9 |  8 || 7 |  6 |  5  |  4 || 3 |  2 |  1  |  0 |
 *  |  0 | LH | D56| D6 ||  0 | LH | D56| D6 || 0 | LH | D56 | D6 || 0 | LH | D56 | D6 |
 *  for each Module:
 *   [1:0] hit with cell information: D6=0x1, D56=0x3
 *   [2]   Low/High threshold: 1 = H, 0=L
 *   e.g.) 0x7 = D5+D6 energy exceeds the high threshold
 *
 * for ADDR,
 *  | 11 |10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
 *  |side|   Trig. Sector  |      SSC     |
 * where side         = 0x0 (A-side), 0x1 (C-side).
 *       trig. sector = 0x(0...2f)
 *       SSC          = 0x(0...12)
 *
 */

class Run2TileMuCoincidenceMap
{
 public:
  Run2TileMuCoincidenceMap(const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey);
  virtual ~Run2TileMuCoincidenceMap() = default;

  Run2TileMuCoincidenceMap(const Run2TileMuCoincidenceMap& right) = default;
  Run2TileMuCoincidenceMap& operator = (const Run2TileMuCoincidenceMap& right) = delete;

  uint16_t getAddr(int16_t side, int16_t sec, int16_t ssc) const;

  uint8_t getFlagPT(const int pt, const int16_t ssc, const int16_t sec, const int16_t side) const;
  uint8_t getFlagROI(const int roi, const int16_t ssc, const int16_t sec, const int16_t side) const;
  uint8_t getTrigMask(const int module, const int16_t ssc, const int16_t sec, const int16_t side) const;

  enum TileMuMask {
    TM_NA    =  0,
    TM_D6_L  =  1,
    TM_D6_H  =  5,
    TM_D56_L =  3,
    TM_D56_H =  7
  };

  enum {N_Input_TileMuModule=4};
  
 protected:
  void dumpMap() const;

 private:
  const SG::ReadCondHandleKey<TGCTriggerData>& m_readCondKey;
};

}   // end of namespace bracket

#endif


