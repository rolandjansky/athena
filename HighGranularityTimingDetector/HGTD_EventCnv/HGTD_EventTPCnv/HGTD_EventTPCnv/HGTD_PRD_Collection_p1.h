/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/HGTD_PRD_Collection_p1.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Since the persistified HGTD_ClusterContainer is a flattended
 *        vector<vector<>>, this helper class is needed to keep track where the
 *        start and end positions of the inner vectors are placed.
 */

#ifndef HGTD_PRD_COLLECTION_P1_H
#define HGTD_PRD_COLLECTION_P1_H

#include "Identifier/IdentifierHash.h"

class HGTD_PRD_Collection_p1 {

public:
  typedef IdentifierHash::value_type IdType_t;

  HGTD_PRD_Collection_p1() : m_hash_id(0), m_size(0){};

  // container cnvs do conversions
  friend class HGTD_ClusterContainerCnv_p1;

private:
  // Hash Identifier of this collection
  IdType_t m_hash_id;

  //  Number of clusters in a given collection
  unsigned short m_size;
};

#endif // HGTD_PRD_COLLECTION_P1_H
