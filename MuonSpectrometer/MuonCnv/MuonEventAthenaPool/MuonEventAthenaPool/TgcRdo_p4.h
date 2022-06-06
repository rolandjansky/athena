/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDO_P4_H
#define MUON_TGCRDO_P4_H

#include <vector>
#include "MuonEventAthenaPool/TgcRawData_p4.h"
#include "CxxUtils/unused.h"

class TgcRdo_p4 : public std::vector<TgcRawData_p4>
{
public:
  // Default constructor
  TgcRdo_p4 () = default;
  friend class TgcRdoCnv_p4;

private:
  uint16_t m_version;

  // ID of this instance
  uint16_t m_id{0};
  // Unused, but shouldn't delete it since it's part of the persistent data.
  uint32_t ATH_UNUSED_MEMBER(m_idHash){0};

  // online IDs
  uint16_t m_subDetectorId{0};
  uint16_t m_rodId{0};

  // Trigger Type
  uint16_t m_triggerType{0};

  // BCID and L1ID on ROD
  uint16_t m_bcId{0};
  uint16_t m_l1Id{0};

  uint32_t m_errors{0};
  uint32_t m_rodStatus{0};
  uint32_t m_localStatus{0};
  uint32_t m_orbit{0};
};

#endif // MUON_TGCRDO_P4_H
