/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCL1RAWDATA_P1_H
#define MUON_TGCL1RAWDATA_P1_H

class TgcL1RawData_p1
{
 public:
 TgcL1RawData_p1() : 
  m_bcTag(0), 
    m_subDetectorId(0), m_srodId(0),
    m_l1Id(0), m_bcId(0), 
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
  
  friend class TgcL1RawDataCnv_p1;

private:

    /********** common attributes
     */

    // BC tag (Central, Next, Previous)
    uint16_t m_bcTag;

    // online Identifiers
    uint16_t m_subDetectorId;
    uint16_t m_srodId;

    // BCID and L1ID on SLB
    uint16_t m_l1Id;
    uint16_t m_bcId;

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

