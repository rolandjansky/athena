/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRAWDATA_P1_H
#define MUON_TGCRAWDATA_P1_H

/*
Persistent representation of the TgcRawData class
Author: Marcin Nowak
        CERN, January 2006
*/

#include <stdint.h>

class TgcRawData_p1
{
public:
    TgcRawData_p1() :
      m_bcTag(0), 
      m_subDetectorId(0), m_rodId(0), m_sswId(0), m_slbId(0), 
      m_l1Id(0), m_bcId(0), 
      m_bitpos(0), 
      m_isCoincidence(false), 
      m_type(0), 
      m_index(0), 
      m_pos(0), m_delta(0)
    {
    }

    friend class TgcRawDataCnv_p1;

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


    /********** Hit attributes
     */

    // bitmap position [40..199]
    uint16_t m_bitpos;


    /********** Coincidence attributes
     */

    // flag for Coincidence-information
    bool m_isCoincidence;

    //  Coincidence type (L-Pt, H-Pt, SL)
    uint16_t m_type;

    // Coincidence index
    uint16_t m_index;

    // Coincidence Data
    uint16_t m_pos;
    uint16_t m_delta;

};

#endif

