/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef STGC_RAWDATA_P1_H
#define STGC_RAWDATA_P1_H

namespace Muon
{
    class STGC_RawData_p1
    {
    public:
    STGC_RawData_p1() : m_id(0), m_time(0), m_charge(0), m_bcTag(0), m_isDead(false) {}
                
        unsigned int        m_id; //!< FIXME! Remove this eventually

	float m_time;

	float m_charge;

	uint16_t m_bcTag;

	bool m_isDead;

    };
}

#endif 
