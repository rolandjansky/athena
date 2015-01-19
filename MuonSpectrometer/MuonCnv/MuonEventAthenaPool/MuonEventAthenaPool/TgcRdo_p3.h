/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDO_P3_H
#define MUON_TGCRDO_P3_H

#include <vector>
#include "MuonEventAthenaPool/TgcRawData_p3.h"

class TgcRdo_p3 : public std::vector<TgcRawData_p3>
{
public:

    // Default constructor
    TgcRdo_p3 () :
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

    friend class TgcRdoCnv_p3;

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

#endif // MUON_TGCRDO_P3_H
