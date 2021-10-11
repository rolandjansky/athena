/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCL1RAWDATA_P1_H
#define MUON_TGCL1RAWDATA_P1_H

class TgcL1RawData_p1
{
 public:
    TgcL1RawData_p1()  =default;
  
  friend class TgcL1RawDataCnv_p1;

private:

    /********** common attributes
     */

    // BC tag (Central, Next, Previous)
    uint16_t m_bcTag{0};

    // online Identifiers
    uint16_t m_subDetectorId{0};
    uint16_t m_srodId{0};

    // BCID and L1ID on SLB
    uint16_t m_l1Id{0};
    uint16_t m_bcId{0};

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
    enum CoincidenceType{
        UnKnown = 0,
        LowPt =  1,
        HighPt = 2,
        StLine = 3,
    };
    uint16_t m_type{CoincidenceType::UnKnown};
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

};

#endif

