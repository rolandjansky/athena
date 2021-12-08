/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTRIGGERLUTS_H
#define TGCTRIGGERLUTS_H

#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <string>
#include <map>
#include <fstream>


class TGCTriggerLUTs {

  friend class TGCTriggerCondAlg;
  friend class TGCTriggerBWCWReader;

 public:

  /// Mask for extracting the side from the GLOBALADDR
  static constexpr uint8_t SIDE_MASK = 0x1;
  /// Bit position of the side bit in the GLOBALADDR
  static constexpr uint8_t SIDE_SHIFT = 29;
  /// Mask for extracting the octant from the GLOBALADDR
  static constexpr uint8_t OCTANT_MASK = 0x7;
  /// Bit position of the octant bits in the GLOBALADDR
  static constexpr uint8_t OCTANT_SHIFT = 26;
  /// Mask for extracting the octant from the GLOBALADDR
  static constexpr uint8_t TYPE_MASK = 0x7;
  /// Bit position of the octant bits in the GLOBALADDR
  static constexpr uint8_t TYPE_SHIFT = 26;
  /// Mask for extracting the phi(F or B) from the GLOBALADDR
  static constexpr uint8_t PHIMOD2_MASK = 0x1;
  /// Bit position of the module number bits in the GLOBALADDR
  static constexpr uint8_t PHIMOD2_SHIFT = 22;
  /// Mask for extracting the module number from the GLOBALADDR
  static constexpr uint8_t MODULE_MASK = 0xf;
  /// Bit position of the module number bits in the GLOBALADDR
  static constexpr uint8_t MODULE_SHIFT = 18;
  /// Mask for extracting the module number from the GLOBALADDR
  static constexpr uint8_t ROI_MASK = 0xff;
  /// Bit position of the module number bits in the GLOBALADDR
  static constexpr uint8_t ROI_SHIFT = 10;
  /// Mask for extracting the deltaR from the GLOBALADDR
  static constexpr uint8_t DR_MASK = 0x1f;
  /// Bit position of the deltaR bits in the GLOBALADDR
  static constexpr uint8_t DR_SHIFT = 4;
  /// Mask for extracting the deltaPhi from the GLOBALADDR
  static constexpr uint8_t DPHI_MASK = 0xf;
  /// Bit position of the deltaPhi bits in the GLOBALADDR
  static constexpr uint8_t DPHI_SHIFT = 0;

  /// Range of DR in the BW coincidence window for 3-station
  static constexpr uint8_t DR_HIGH_RANGE = 15;
  /// Range of DR in the BW coincidence window for 2-station
  static constexpr uint8_t DR_LOW_RANGE = 7;
  /// Range of DPhi in the BW coincidence window for 3-station
  static constexpr uint8_t DPHI_HIGH_RANGE = 7;
  /// Range of DPhi in the BW coincidence window for 2-station
  static constexpr uint8_t DPHI_LOW_RANGE = 3;

  enum { COIN_HH = 0x0, COIN_HL = 0x1, COIN_LH = 0x2, COIN_LL = 0x3 };
  enum {CW_BW=0, CW_EIFI=1, CW_TILE=2, CW_NSW=3, CW_BIS78=4, WHICH_INNER=5, HOTRoI=6, LUT_NUM}; 
  enum {N_PT_THRESH=15,
        N_SIDE=2,
        N_OCTANT=8,
        N_MODULETYPE=12};

  TGCTriggerLUTs();
  virtual ~TGCTriggerLUTs();

  std::string getData(int cwtype, std::string file) const;
  std::string getData(int cwtype, int channel) const;
  std::string getFile(int cwtype, int channel) const;
  std::string getVersion(int cwtype, int channel = 0) const;
  std::string getType(int cwtype, int channel = 0) const;

  int8_t getBigWheelPt(const uint32_t addr) const;

 private:
  std::map<std::string, std::string> m_datamap[LUT_NUM];
  std::vector<std::string> m_data[LUT_NUM];
  std::vector<std::string> m_file[LUT_NUM];
  std::vector<std::string> m_version[LUT_NUM];
  std::vector<std::string> m_type[LUT_NUM];

  std::unordered_map<uint32_t, int8_t> m_ptmap_bw;
};


CLASS_DEF(TGCTriggerLUTs, 206388330, 1)

#include "AthenaKernel/CondCont.h"
CLASS_DEF(CondCont<TGCTriggerLUTs>, 123453230, 1)

#endif // TGCTRIGGERDATA_H
