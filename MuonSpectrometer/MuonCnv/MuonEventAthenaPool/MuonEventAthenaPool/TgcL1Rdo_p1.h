/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCL1RDO_P1_H
#define MUON_TGCL1RDO_P1_H

#include <vector>
#include "MuonEventAthenaPool/TgcL1RawData_p1.h"
#include "CxxUtils/unused.h"

class TgcL1Rdo_p1 : public std::vector<TgcL1RawData_p1>
{
public:

    // Default constructor
    TgcL1Rdo_p1 () :
    m_version(300),
    m_id(0),
    m_idHash(0),  
    m_subDetectorId(0),
    m_srodId(0),
    m_triggerType(0),
    m_bcId(0),
    m_l1Id(0),
    m_errors(0),
    m_srodStatus(0),
    m_localStatus(0),
    m_orbit(0)
    {
    }

    friend class TgcL1RdoCnv_p1;

private:

    uint16_t m_version;

    // ID of this instance
    uint16_t m_id;
    // Unused, but shouldn't delete it since it's part of the persistent data.
    uint32_t ATH_UNUSED_MEMBER(m_idHash);


    // online IDs
    uint16_t m_subDetectorId;
    uint16_t m_srodId;

    // Trigger Type
    uint16_t m_triggerType;

    // BCID and L1ID on ROD
    uint16_t m_bcId;
    uint16_t m_l1Id;


    uint32_t m_errors;
    uint32_t m_srodStatus;
    uint32_t m_localStatus;
    uint32_t m_orbit;
};

#endif // MUON_TGCL1RDO_P1_H
