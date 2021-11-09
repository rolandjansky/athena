/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_PrepRawData/HGTD_ClusterContainer.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 *
 * @brief The container holds all clusters found in HGTD, the individual
 *        collections hold the clusters on specific sensors
 */

#ifndef HGTD_PREPRAWDATA_HGTD_CLUSTERCONTAINER_H
#define HGTD_PREPRAWDATA_HGTD_CLUSTERCONTAINER_H

#include "AthLinks/DeclareIndexingPolicy.h"
#include "HGTD_PrepRawData/HGTD_ClusterCollection.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"

typedef Trk::PrepRawDataContainer<HGTD_ClusterCollection> HGTD_ClusterContainer;

CLASS_DEF(HGTD_ClusterContainer, 1124691928, 1)
CONTAINER_IS_IDENTCONT(HGTD_ClusterContainer)

#endif // HGTD_PREPRAWDATA_HGTD_CLUSTERCONTAINER_H
