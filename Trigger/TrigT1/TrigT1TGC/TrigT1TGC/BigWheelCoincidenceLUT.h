/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_BigWheelCoincidenceLUT_h
#define TrigT1TGC_BigWheelCoincidenceLUT_h

#include <vector>
#include <map>
#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TGCTriggerCondSvc/TGCTriggerLUTs.h"
#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGC {

/** Contents of Run-3 BW-CW LUT
 *  ===========================
 *   std::unordered_map<GLOBALADDR, PTCHAR>
 *  where
 *   GLOBALADDR | 27 bits | uint32_t  | side, octant, type, phimod2, module, roi,
 *                                    | DR(0...0x1f for -15...15), DPhi(0...0xf for -7...7)
 *   PTCHAR     |  5 bits | char      | pT (4 bits) and charge
 *
 *  for GLOBALADDR
 *  | 29 |28|27|26|25|24|23|   22  |21|..|18|17|...|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
 *  |side| octant | 0|type |phimod2| module |   RoI   | 0|   delta R    | delta Phi |
 *  where side   = 0x0 (A-side), 0x1 (C-side).
 *        octant = 0x(0...7)
 *        type   = 0x0(HH), 0x1(HL), 0x2(LH), 0x3(LL): HL means 3-sta.-wire and 2-sta.-strip.
 */

class BigWheelCoincidenceLUT {
 public:
  BigWheelCoincidenceLUT(LVL1TGCTrigger::TGCArguments*,
                         const SG::ReadCondHandleKey<TGCTriggerLUTs>& readKey,
                         const std::string& version);
  virtual ~BigWheelCoincidenceLUT();

  BigWheelCoincidenceLUT(const BigWheelCoincidenceLUT& right) = default;
  BigWheelCoincidenceLUT& operator = (const BigWheelCoincidenceLUT& right) = delete;

  bool readMap();

  int8_t test(int octantId, int moduleId, int subsector,
              int type, int dr, int dphi) const;

  int   getMapType(int hlwire, int hlstrip) const;
  const std::string& getVersion() const;
  int   getOctantId() const;

  const LVL1TGCTrigger::TGCArguments* tgcArgs() const;

 protected:
  int getTYPE(int lDR, int hDR, int lDPhi, int hDPhi) const;

 private:  // hide default constructor
  BigWheelCoincidenceLUT() = delete;

 protected:
  static constexpr uint32_t N_COIN_TYPE = 4;

  //converter from char to pTthre Number. the sign mean muon sign.
  std::map<char, int8_t> m_pTdef={{'X',0},
              {'A',1},{'B',2},{'C',3},{'D',4},{'E',5},{'F',6},{'G',7},{'H',8},{'I',9},{'J',10},{'K',11},{'L',12},{'M',13},{'N',14},{'O',15},
              {'a',-1},{'b',-2},{'c',-3},{'d',-4},{'e',-5},{'f',-6},{'g',-7},{'h',-8},{'i',-9},{'j',-10},{'k',-11},{'l',-12},{'m',-13},{'n',-14},{'o',-15} };

 private:
  std::unordered_map<uint32_t, char> m_lut;   // GLOBALADDR, PTCHAR

  std::string m_verName;
  int m_side;
  int m_octant;
  bool m_fullCW;
  
  LVL1TGCTrigger::TGCArguments* m_tgcArgs;

  const SG::ReadCondHandleKey<TGCTriggerLUTs>& m_readCondKey;
};

inline const LVL1TGCTrigger::TGCArguments* BigWheelCoincidenceLUT::tgcArgs() const {
  return m_tgcArgs;
}

inline const std::string& BigWheelCoincidenceLUT::getVersion() const {
  return m_verName;
}

inline int BigWheelCoincidenceLUT::getOctantId() const {
  return m_octant;
}

inline int BigWheelCoincidenceLUT::getTYPE(int lDR, int hDR, int lDPhi, int hDPhi) const {
  if((lDR == -TGCTriggerLUTs::DR_HIGH_RANGE) && (hDR == TGCTriggerLUTs::DR_HIGH_RANGE)) {
    if     ((lDPhi == -TGCTriggerLUTs::DPHI_HIGH_RANGE) && (hDPhi == TGCTriggerLUTs::DPHI_HIGH_RANGE)) return TGCTriggerLUTs::COIN_HH;
    else if((lDPhi == -TGCTriggerLUTs::DPHI_LOW_RANGE) && (hDPhi == TGCTriggerLUTs::DPHI_LOW_RANGE))   return TGCTriggerLUTs::COIN_HL;
  } else if((lDR == -TGCTriggerLUTs::DR_LOW_RANGE) && (hDR == TGCTriggerLUTs::DR_LOW_RANGE)) {
    if     ((lDPhi == -TGCTriggerLUTs::DPHI_HIGH_RANGE) && (hDPhi == TGCTriggerLUTs::DPHI_HIGH_RANGE)) return TGCTriggerLUTs::COIN_LH;
    else if((lDPhi == -TGCTriggerLUTs::DPHI_LOW_RANGE) && (hDPhi == TGCTriggerLUTs::DPHI_LOW_RANGE))   return TGCTriggerLUTs::COIN_LL;
  }
  return -1;
}

inline int BigWheelCoincidenceLUT::getMapType(int hlwire, int hlstrip) const {
  return (TGCTriggerLUTs::COIN_LL - 2*hlwire - hlstrip);
}

}  // namespace LVL1TGC

#endif   // TrigT1TGC_BigWheelCoincidenceLUT_H


