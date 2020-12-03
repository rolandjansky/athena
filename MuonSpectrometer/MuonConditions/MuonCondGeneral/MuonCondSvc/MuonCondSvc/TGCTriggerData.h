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

class TGCTriggerData
{
 friend class TGCTriggerDbAlg;

 public:
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

  const std::map<unsigned short, std::map<unsigned short, unsigned char>>& getPtMapBw(const unsigned char side, const unsigned char octant) const;

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

  /** Contents of Run-2 BW-CW LUT
   *  ===========================
   *     map<OCTANT, map<SUBSECADDR, map<DELTAADDR, PTVALUE> > >
   *  where
   *   OCTANT     |  4 bits | unsigned char  | side(A=0, C=1)<<3 & octant(0...7)
   *   SUBSECADDR | 16 bits | unsigned short | consists of type, phimod2, module, and roi
   *   DELTAADDR  |  9 bits | unsigned short | DR(0...0x1f for -15...15)<<4 & DPhi(0...0xf for -7...7)
   *   PTVALUE    |  3 bits | unsigned char  | pT value (0x0 and 0x7 is no cand.)
   *
   *  for SUBSECADDR
   *  | 15 | 14 | 13 |    12   | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
   *  |  0 |  type   | phimod2 |      module     |              RoI              |
   *  where type = 0x0 (HH), 0x1 (HL), 0x2 (LH), 0x3 (LL): HL means 3-station-wire and 2-station-strip.
   *  This may reduce the malloc size when the fullCW option is not used.
   */
  std::map<unsigned char, std::map<unsigned short, std::map<unsigned short, unsigned char>>> m_ptmap_bw;

//  std::map<int, std::map<int,int> > m_readmap_bw[N_SIDE][N_OCTANT][N_PT_THRESH];

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
