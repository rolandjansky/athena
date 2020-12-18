/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTRIGGERDATA_H
#define TGCTRIGGERDATA_H

#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <string>
#include <map>
#include <vector>


/** Contents of Run-2 BW-CW LUT
 *  ===========================
 *   std::unordered_map<GLOBALADDR, PTVALUE>
 *  where
 *   GLOBALADDR | 27 bits | unsigned int  | side, octant, type, phimod2, module, roi,
 *                                        | DR(0...0x1f for -15...15)<<4 & DPhi(0...0xf for -7...7)
 *   PTVALUE    |  3 bits | unsigned char | pT value (0x0 and 0x7 is no cand.)
 *
 *  for GLOBALADDR
 *  | 29 |28|27|26|25|24|23|   22  |21|20|19|18|17|16|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
 *  |side| octant | 0|type |phimod2|  module   |          RoI          | 0|   delta R    | delta Phi |
 *  where side   = 0x0 (A-side), 0x1 (C-side).
 *        octant = 0x(0...7)
 *        type   = 0x0 (HH), 0x1 (HL), 0x2 (LH), 0x3 (LL): HL means 3-station-wire and 2-station-strip.
 */

class TGCTriggerData
{
 friend class TGCTriggerDbAlg;

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
  /// Mask for pT value for Run-2
  static constexpr uint8_t PT_MASK = 0x7;

  /// Range of DR in the BW coincidence window for 3-station
  static constexpr uint8_t DR_HIGH_RANGE = 15;
  /// Range of DR in the BW coincidence window for 2-station
  static constexpr uint8_t DR_LOW_RANGE = 7;
  /// Range of DPhi in the BW coincidence window for 3-station
  static constexpr uint8_t DPHI_HIGH_RANGE = 7;
  /// Range of DPhi in the BW coincidence window for 2-station
  static constexpr uint8_t DPHI_LOW_RANGE = 3;

  enum {CW_BW=0, CW_EIFI=1, CW_TILE=2, CW_NUM=3};
  enum {N_PT_THRESH=6,
        N_SIDE=2,
        N_OCTANT=8,
        N_ENDCAP_SSC=19,
        N_ENDCAP_SECTOR=48,
        N_ROI_IN_SSC=8,
        N_EIFI_INPUT=4,
        N_EIFI_REGION=2,
        N_EIFI_READOUT=2,
        N_EIFI_TRIGBIT=4,
        N_TILE_INPUT=4};
  enum {COIN_HH=0x0, COIN_HL=0x1, COIN_LH=0x2, COIN_LL=0x3};

  TGCTriggerData();
  virtual ~TGCTriggerData();

  std::string getData(int cwtype, std::string file) const;
  std::string getData(int cwtype, int channel) const;
  std::string getFile(int cwtype, int channel) const;
  std::string getVersion(int cwtype, int channel = 0) const;
  std::string getType(int cwtype, int channel = 0) const;
  bool isActive(int cwtype, int channel = 0) const;

  int8_t getTYPE(const int16_t lDR, const int16_t hDR, const int16_t lDPhi, const int16_t hDPhi) const;

  uint8_t getBigWheelPt(const uint32_t addr) const;

  unsigned short getTrigBitEifi(int side, int slot, int ssc, int sectorId) const;
  unsigned char getFlagPtEifi(int side, int ssc, int sectorId) const;
  unsigned char getFlagRoiEifi(int side, int ssc, int sectorId) const;

  unsigned short getTrigMaskTile(int ssc, int sectorId, int side) const;
  unsigned char getFlagPtTile(int ssc, int sectorId, int side) const;
  unsigned char getFlagRoiTile(int ssc, int sectorId, int side) const;

 private:
  std::map<std::string, std::string> m_datamap[CW_NUM];
  std::vector<std::string> m_data[CW_NUM];
  std::vector<std::string> m_file[CW_NUM];
  std::vector<std::string> m_version[CW_NUM];
  std::vector<std::string> m_type[CW_NUM];
  std::vector<bool>        m_active[CW_NUM];

  /// Run-2 BW-CW LUT map
  std::unordered_map<uint32_t, uint8_t> m_ptmap_bw;

  /** Bit information of Run-2 EIFI-LUT
   *  =================================
   *  bits for each input
   *  |                   EI                ||           FI                  |
   *  |       Strip       |        Wire     ||     Strip     |     Wire      |
   *  | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 || 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
   *  | <--L   phi   S--> | <--L  eta  S--> || <--L phi S--> | <--L eta  S-->|
   *  where each sensor consists of 4 bits information.
   *
   *  Note: Only if fullCW option is true, the different db file per side is read.
   *        Therefore, the contents are stored by std::vector of side index (A=0, C=1).
   */
  std::vector<unsigned short> m_trigbit_eifi[N_ENDCAP_SECTOR][N_ENDCAP_SSC][N_EIFI_INPUT];
  std::vector<unsigned char> m_flagpt_eifi[N_ENDCAP_SECTOR][N_ENDCAP_SSC];   //< bits for applied pT (N_PT_THRESH)
  std::vector<unsigned char> m_flagroi_eifi[N_ENDCAP_SECTOR][N_ENDCAP_SSC];  //< bits for applied RoI in SSC (N_ROI_IN_SSC)

  /** Bit information of Run-2 Tile-LUT
   *  =================================
   *  bits for each input
   *  | Module D (Tile=3) || Module C (Tile=2) || Module B (Tile=1) || Module A (Tile=0) |
   *  | 15 | 14 | 13 | 12 || 11 | 10 |  9 |  8 || 7 |  6 |  5  |  4 || 3 |  2 |  1  |  0 |
   *  |  0 | LH | D56| D6 ||  0 | LH | D56| D6 || 0 | LH | D56 | D6 || 0 | LH | D56 | D6 |
   *  for each Module:
   *   [1:0] hit with cell information: D6=0x1, D56=0x3
   *   [2]   Low/High threshold: 1 = H, 0=L
   *   e.g.) 0x7 = D5+D6 energy exceeds the high threshold
   *
   *   Note: One db file consists of whole sector information as a fullCW.
   */
  unsigned short m_trigbit_tile[N_ENDCAP_SSC][N_ENDCAP_SECTOR][N_SIDE];
  unsigned char m_flagpt_tile[N_ENDCAP_SSC][N_ENDCAP_SECTOR][N_SIDE];   //< bits for applied pT (N_PT_THRESH)
  unsigned char m_flagroi_tile[N_ENDCAP_SSC][N_ENDCAP_SECTOR][N_SIDE];  //< bits for applied RoI in SSC (N_ROI_IN_SSC)

};

CLASS_DEF(TGCTriggerData, 72345188, 2)

#include "AthenaKernel/CondCont.h"
CLASS_DEF(CondCont<TGCTriggerData>, 96649668, 2)

#endif // TGCTRIGGERDATA_H
