/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDO_P2_H
#define MUON_TGCRDO_P2_H

/*

Persistent represenation of the TgcRdo class
Author: Zvi Tarem
        CERN, August, 2006

*/

#include <vector>
#include "MuonEventAthenaPool/TgcRawData_p2.h"

class TgcRdo_p2 : public std::vector<TgcRawData_p2>
{
public:

    // Default constructor
    TgcRdo_p2 () :
    m_version(300),
    m_id(0),
    m_idHash(0),  
    m_subDetectorId(0),
    m_rodId(0),
    m_triggerType(0),
    m_bcId(0),
    m_l1Id(0),
    m_errors(0),
    m_rodStatus(0),
    m_localStatus(0),
    m_orbit(0)
    {
    }

    friend class TgcRdoCnv_p2;

private:

    uint16_t m_version;

    // ID of this instance
    uint16_t m_id;
    uint32_t m_idHash; 


    // online IDs
    uint16_t m_subDetectorId;
    uint16_t m_rodId;

    // Trigger Type
    uint16_t m_triggerType;

    // BCID and L1ID on ROD
    uint16_t m_bcId;
    uint16_t m_l1Id;


    uint32_t m_errors;
    uint32_t m_rodStatus;
    uint32_t m_localStatus;
    uint32_t m_orbit;
};

#endif // MUON_TGCRDO_P2_H
