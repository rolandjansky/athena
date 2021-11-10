/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/HGTD_ClusterContainer_p1.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief In the persistent cluster container, the individual collections of the
          clusters are flattened into one single vector. A helper class holds
          the identifiers and separator positions to rebuild the container.
 */

#ifndef HGTD_CLUSTERCONTAINER_P1_H
#define HGTD_CLUSTERCONTAINER_P1_H

#include "HGTD_EventTPCnv/HGTD_Cluster_p1.h"
#include "HGTD_EventTPCnv/HGTD_PRD_Collection_p1.h"
#include <string>
#include <vector>

class HGTD_ClusterContainer_p1 {
public:
  /// Default constructor
  HGTD_ClusterContainer_p1() = default;

  // holds information to restore the individual collections from the flattened
  // list of persistified clusters
  std::vector<HGTD_PRD_Collection_p1> m_collection_separator;

  std::vector<HGTD_Cluster_p1> m_cluster_list;
};

#endif
