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
#include "MuonCondSvc/TGCTriggerData.h"
#include "TGCTriggerCondSvc/TGCTriggerLUTs.h"
#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGC {

/** Contents of Run-3 BW-CW LUT
 *  ===========================
 *   std::unordered_map<GLOBALADDR, PTVALUE>
 *  where
 *   GLOBALADDR | 27 bits | unsigned int  | side, octant, type, phimod2, module, roi,
 *                                        | DR(0...0x1f for -15...15), DPhi(0...0xf for -7...7)
 *   PTVALUE    |  3 bits | unsigned char | pT value (0x0 and 0x7 is no cand.)
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

  int test(int octantId, int moduleId, int subsector,
           int type, int dr, int dphi) const;

  int   getMapType(int hlwire, int hlstrip) const;
  const std::string& getVersion() const;
  int   getOctantId() const;

  const LVL1TGCTrigger::TGCArguments* tgcArgs() const;

 private: // hide default constructor
  BigWheelCoincidenceLUT() = delete;

 protected:
  int SUBSECTORADD(int ssid, int modid, int phimod2, int type) const;
  int getTYPE(int lDR, int hDR, int lDPhi, int hDPhi ) const;

  enum {TMap_HH=0, TMap_HL, TMap_LH, TMap_LL, N_TMap};
  enum {NumberOfCoincidenceType=4};
  enum {NumberOfModuleType=12};
  enum {DR_offset=-15, DPhi_offset=-7};

 //converter from char to pTthre Number. the sign mean muon sign.
  std::map<char, int> m_pTdef={{'X',0},
              {'A',1},{'B',2},{'C',3},{'D',4},{'E',5},{'F',6},{'G',7},{'H',8},{'I',9},{'J',10},{'K',11},{'L',12},{'M',13},{'N',14},{'O',15},
              {'a',-1},{'b',-2},{'c',-3},{'d',-4},{'e',-5},{'f',-6},{'g',-7},{'h',-8},{'i',-9},{'j',-10},{'k',-11},{'l',-12},{'m',-13},{'n',-14},{'o',-15} };

 private:
  std::map<int, std::map<int, std::map<int, char> > > m_lut;   //for Run3 <RoI(&type),<R,<Phi,pT(char)> > >

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
  if((lDR == -TGCTriggerData::DR_HIGH_RANGE) && (hDR == TGCTriggerData::DR_HIGH_RANGE)) {
    if     ((lDPhi == -TGCTriggerData::DPHI_HIGH_RANGE) && (hDPhi == TGCTriggerData::DPHI_HIGH_RANGE)) return TGCTriggerData::COIN_HH;
    else if((lDPhi == -TGCTriggerData::DPHI_LOW_RANGE) && (hDPhi == TGCTriggerData::DPHI_LOW_RANGE))   return TGCTriggerData::COIN_HL;
  } else if((lDR == -TGCTriggerData::DR_LOW_RANGE) && (hDR == TGCTriggerData::DR_LOW_RANGE)) {
    if     ((lDPhi == -TGCTriggerData::DPHI_HIGH_RANGE) && (hDPhi == TGCTriggerData::DPHI_HIGH_RANGE)) return TGCTriggerData::COIN_LH;
    else if((lDPhi == -TGCTriggerData::DPHI_LOW_RANGE) && (hDPhi == TGCTriggerData::DPHI_LOW_RANGE))   return TGCTriggerData::COIN_LL;
  }
  return -1;
}

inline int BigWheelCoincidenceLUT::getMapType(int hlwire, int hlstrip) const {
  return (N_TMap - 2*hlwire - hlstrip - 1);
}


inline int BigWheelCoincidenceLUT::SUBSECTORADD(int ssid, int modid, int phimod2, int type) const {
  return (ssid+(modid<<8)+(phimod2<<12) + (type<<16) );
}

}  // namespace LVL1TGC

#endif   // TrigT1TGC_BigWheelCoincidenceLUT_H


