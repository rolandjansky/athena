/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCTileMuCoincidenceLUT_h
#define TrigT1TGC_TGCTileMuCoincidenceLUT_h

#include "TrigT1TGC/TGCArguments.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

#include <map>
#include <string>

namespace LVL1TGC {

/**
 * @brief This class stores the LUT for Tile-Muon coincidence of the Run-3 L1Muon Endcap trigger.
 * The contents are read from the Condition database, and are kept in the XXXX object.
 * When the coincidence map is read from the text file, the contents is kept in member variables
 * of this class.
 * Note that the structure is temporary same as Run-2's one.
 */

/** Contents of Run-3 Tile-Muon LUT
 * ================================
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

class TGCTileMuCoincidenceLUT
{
 public:
  TGCTileMuCoincidenceLUT(LVL1TGCTrigger::TGCArguments*,
                          const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                          const std::string& version="NA");
  virtual ~TGCTileMuCoincidenceLUT();

  TGCTileMuCoincidenceLUT(const TGCTileMuCoincidenceLUT& right);
  TGCTileMuCoincidenceLUT& operator = (const TGCTileMuCoincidenceLUT& right) = delete;

  uint16_t getAddr(int16_t side, int16_t sec, int16_t ssc) const;

  int getFlagPT(const int pt, const int16_t ssc, const int16_t sec, const int16_t side) const;
  int getFlagROI(const int roi, const int16_t ssc, const int16_t sec, const int16_t side) const;
  int getTrigMask(const int module, const int16_t ssc, const int16_t sec, const int16_t side) const;

  const std::string&  getVersion() const { return m_verName; }
  bool readMap();

  enum TileMuMask {
    TM_NA    =  0,
    TM_D6_L  =  1,
    TM_D6_H  =  5,
    TM_D56_L =  3,
    TM_D56_H =  7
  };

  enum {N_Input_TileMuModule=4};
  
  LVL1TGCTrigger::TGCArguments* tgcArgs() { return m_tgcArgs; }
  const LVL1TGCTrigger::TGCArguments* tgcArgs() const { return m_tgcArgs; }

 private:
  std::unordered_map<uint16_t, uint8_t> m_flagpt;
  std::unordered_map<uint16_t, uint8_t> m_flagroi;
  std::unordered_map<uint16_t, uint16_t> m_trigbit;

  std::string m_verName;
  LVL1TGCTrigger::TGCArguments* m_tgcArgs;
  const SG::ReadCondHandleKey<TGCTriggerData>& m_readCondKey;
};

}   // end of namespace

#endif


