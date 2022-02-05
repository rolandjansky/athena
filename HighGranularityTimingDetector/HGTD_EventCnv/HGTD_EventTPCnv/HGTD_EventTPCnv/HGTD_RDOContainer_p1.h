/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/HGTD_RDOContainer_p1.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief
 */

#ifndef HGTD_RDOCONTAINER_P1_H
#define HGTD_RDOCONTAINER_P1_H

#include "HGTD_EventTPCnv/HGTD_RDOCollection_p1.h"
#include "HGTD_EventTPCnv/HGTD_RDO_p1.h"
#include <string>
#include <vector>

class HGTD_RDOContainer_p1 {
public:
  /// Default constructor
  HGTD_RDOContainer_p1() = default;

  // holds information to restore the individual collections from the flattened
  // list of persistified clusters
  std::vector<HGTD_RDOCollection_p1> m_collection_separator;

  std::vector<HGTD_RDO_p1> m_rdo_list;
};

#endif
