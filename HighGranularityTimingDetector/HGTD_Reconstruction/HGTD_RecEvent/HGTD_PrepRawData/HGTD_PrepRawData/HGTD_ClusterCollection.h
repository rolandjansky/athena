/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_PrepRawData/HGTD_ClusterCollection.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Clusters on a given sensor are grouped in this identifiable
 *        collection.
 */

#ifndef HGTD_PREPRAWDATA_HGTD_CLUSTERCOLLECTION_H
#define HGTD_PREPRAWDATA_HGTD_CLUSTERCOLLECTION_H

#include "HGTD_PrepRawData/HGTD_Cluster.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"

// Containers
typedef Trk::PrepRawDataCollection<HGTD_Cluster> HGTD_ClusterCollection;

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator<<(MsgStream& sl, const HGTD_ClusterCollection& coll);

/**Overload of << operator for std::ostream for debug output*/
std::ostream& operator<<(std::ostream& sl, const HGTD_ClusterCollection& coll);

CLASS_DEF(HGTD_ClusterCollection, 1309033586, 1)

#endif // HGTD_PREPRAWDATA_HGTD_CLUSTERCOLLECTION_H
