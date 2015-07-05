/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRAWDATA_P3_H
#define MUON_TGCRAWDATA_P3_H

class TgcRawData_p3
{
 public:
 TgcRawData_p3() : 
  m_bcTag(0), 
    m_subDetectorId(0), m_rodId(0), m_sswId(0), m_slbId(0), 
    m_l1Id(0), m_bcId(0), 
    m_slbType(0), 
    m_bitpos(0), 
    m_tracklet(0), 
    m_adjacent(false), 
    m_type(0), m_forward(false), 
    m_index(0), 
    m_pos(0), m_delta(0), m_segment(0), m_subMatrix(0), 
    m_sector(0), m_chip(0), m_hipt(false), m_hitId(0), m_hsub(0), m_strip(false), m_inner(0), 
    m_cand3plus(false), m_muplus(false), m_threshold(0), m_overlap(false), m_veto(false), m_roi(0)  
    {
    }
  
  friend class TgcRawDataCnv_p3;

private:

    /********** common attributes
     */

    // BC tag (Central, Next, Previous)
    uint16_t m_bcTag;

    // online Identifiers
    uint16_t m_subDetectorId;
    uint16_t m_rodId;
    uint16_t m_sswId;
    uint16_t m_slbId;

    // BCID and L1ID on SLB
    uint16_t m_l1Id;
    uint16_t m_bcId;

    uint16_t m_slbType;

    /********** Hit attributes
     */

    // bitmap position [40..199]
    uint16_t m_bitpos;

    // Index of assocaitated coincidence (if any) [0..63]
    uint16_t m_tracklet;

    bool m_adjacent;

    /********** Coincidence attributes
     */

    //  Coincidence type (L-Pt, H-Pt, SL)
    uint16_t m_type;
    bool m_forward;

    // Coincidence index
    uint16_t m_index;

    // Coincidence Data
    uint16_t m_pos;
    int16_t m_delta;
    uint16_t m_segment;
    uint16_t m_subMatrix;

    // HipT data
    uint16_t m_sector;
    uint16_t m_chip;
    bool m_hipt;
    uint16_t m_hitId;
    uint16_t m_hsub;
    bool m_strip;
    int m_inner;

    // Sector Logic
    bool m_cand3plus;
    bool m_muplus;
    uint16_t m_threshold;
    bool m_overlap;
    bool m_veto;
    uint16_t m_roi;
};

#endif

