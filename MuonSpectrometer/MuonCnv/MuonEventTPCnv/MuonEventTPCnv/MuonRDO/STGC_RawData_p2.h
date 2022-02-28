/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef STGC_RAWDATA_P2_H
#define STGC_RAWDATA_P2_H

namespace Muon
{
    class STGC_RawData_p2
    {
    public:
    STGC_RawData_p2() : m_id(0), m_time(0), m_charge(0), m_bcTag(0), m_isDead(false) {}
                
        unsigned int        m_id; //!< FIXME! Remove this eventually

        unsigned int m_time;

        unsigned int m_charge;

        uint16_t m_bcTag;

        bool m_isDead;

    };
}

#endif 
