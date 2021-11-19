/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/HGTD_RDO_p1.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief Represents the persitified version of HGTD_RDO.
 */

#ifndef HGTD_EVENTTPCNV_HGTD_RDO_P1_H
#define HGTD_EVENTTPCNV_HGTD_RDO_P1_H

#include "HGTD_RawData/HGTD_RDO.h"
#include "Identifier/Identifier.h"

class HGTD_RDO_p1 {
public:
  typedef Identifier::value_type IdType_t;

  HGTD_RDO_p1() = default;

  friend class HGTD_RDOCnv_p1;

private:
  IdType_t m_rdo_id;
  float m_toa;
  int m_tot;
  unsigned short m_bcid;
  unsigned short m_l1_id;
  unsigned short m_l1_a;
};

#endif // HGTD_RDO_P1_H
