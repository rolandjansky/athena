/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRAWDATA_P4_H
#define MUON_TGCRAWDATA_P4_H

class TgcRawData_p4
{
 public:
  TgcRawData_p4() = default;
  friend class TgcRawDataCnv_p4;

private:

  /********** common attributes
   */

  // BC tag (Central, Next, Previous)
  uint16_t m_bcTag{0};

  // online Identifiers
  uint16_t m_subDetectorId{0};
  uint16_t m_rodId{0};
  uint16_t m_sswId{0};
  uint16_t m_slbId{0};

  // BCID and L1ID on SLB
  uint16_t m_l1Id{0};
  uint16_t m_bcId{0};
  uint16_t m_slbType{0};

  /********** Hit attributes
   */

  // bitmap position [40..199]
  uint16_t m_bitpos{0};

  // Index of assocaitated coincidence (if any) [0..63]
  uint16_t m_tracklet{0};

  bool m_adjacent{0};

  /********** Coincidence attributes
   */

  //  Coincidence type (L-Pt, H-Pt, SL)
  uint16_t m_type{0};
  bool m_forward{0};

  // Coincidence index
  uint16_t m_index{0};

  // Coincidence Data
  uint16_t m_pos{0};
  int16_t m_delta{0};
  uint16_t m_segment{0};
  uint16_t m_subMatrix{0};

  // HipT data
  uint16_t m_sector{0};
  uint16_t m_chip{0};
  bool m_hipt{false};
  uint16_t m_hitId{0};
  uint16_t m_hsub{0};
  bool m_strip{false};
  int m_inner{0};
  
  // Sector Logic
  bool m_cand3plus{false};
  bool m_muplus{false};
  uint16_t m_threshold{0};
  bool m_overlap{false};
  bool m_veto{false};
  uint16_t m_roi{0};

  // New Sector Logic
  // RoI
  uint16_t m_innerflag{0};
  uint16_t m_coinflag{0};

  // NSW
  uint16_t m_nsweta{0};
  uint16_t m_nswphi{0};
  uint16_t m_nswsl{0};
  uint16_t m_nswcand{0};
  uint16_t m_nswdtheta{0};
  uint16_t m_nswphires{0};
  uint16_t m_nswlowres{0};
  uint16_t m_nswid{0};
  
  // RPC BIS78
  uint16_t m_rpceta{0};
  uint16_t m_rpcphi{0};
  uint16_t m_rpcflag{0};
  uint16_t m_rpcdeta{0};
  uint16_t m_rpcdphi{0};
  
  // EIFI
  uint16_t m_ei{0};
  uint16_t m_fi{0};
  uint16_t m_cid{0};
  
    // TMDB
  uint16_t m_tmdbmod{0};
  uint16_t m_tmdbbcid{0};
};

#endif
