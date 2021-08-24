/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    // Default constructor
    TgcL1Rdo_p1 () = default;

    friend class TgcL1RdoCnv_p1;

private:

    uint16_t m_version;

    // ID of this instance
    uint16_t m_id{0};
    // Unused, but shouldn't delete it since it's part of the persistent data.
    uint32_t ATH_UNUSED_MEMBER(m_idHash){0};


    // online IDs
    uint16_t m_subDetectorId{0};
    uint16_t m_srodId{0};

    // Trigger Type
    uint16_t m_triggerType{0};

    // BCID and L1ID on ROD
    uint16_t m_bcId{0};
    uint16_t m_l1Id{0};


    uint32_t m_errors{0};
    uint32_t m_srodStatus{0};
    uint32_t m_localStatus{0};
    uint32_t m_orbit{0};

};

#endif // MUON_TGCL1RDO_P1_H
