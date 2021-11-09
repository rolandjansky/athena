/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_PrepRawData/src/HGTD_ClusterCollection.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Implementation file for HGTD_ClusterCollection.
 */

#include "HGTD_PrepRawData/HGTD_ClusterCollection.h"
#include "GaudiKernel/MsgStream.h"

MsgStream& operator<<(MsgStream& sl, const HGTD_ClusterCollection& coll) {
  sl << "HGTD_ClusterCollection: "
     << "identify()="
     << coll.identify()
     << ", PRD=[";
  HGTD_ClusterCollection::const_iterator it = coll.begin();
  HGTD_ClusterCollection::const_iterator itEnd = coll.end();
  for (; it != itEnd; ++it)
    sl << (**it) << ", ";
  sl << " ]" << std::endl;
  return sl;
}

std::ostream& operator<<(std::ostream& sl, const HGTD_ClusterCollection& coll) {
  sl << "HGTD_ClusterCollection: "
     << "identify()="
     << coll.identify()
     << ", PRD=[";
  HGTD_ClusterCollection::const_iterator it = coll.begin();
  HGTD_ClusterCollection::const_iterator itEnd = coll.end();
  for (; it != itEnd; ++it)
    sl << (**it) << ", ";
  sl << " ]" << std::endl;
  return sl;
}
