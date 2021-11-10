/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RawData/HGTD_RDOCollections.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief
 * FIXME: not sure yet what to do with assignment and copy? make all private
 * as done in InDet? need to know why first...
 */

#ifndef HGTD_RAWDATA_HGTD_RDOCOLLECTION_H
#define HGTD_RAWDATA_HGTD_RDOCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "HGTD_RawData/HGTD_RDO.h"
#include "Identifier/IdentifierHash.h"

class HGTD_RDOCollection : public DataVector<HGTD_RDO> {
  // friend class HGTD_RDORawDataCollectionCnv_p1; //FIXME probably later

public:
  /**
   * @brief Default constructor should NOT be used, but is needed for pool I/O.
   */
  HGTD_RDOCollection() = default;

  HGTD_RDOCollection(IdentifierHash hash) : m_id_hash(hash) {}

  void setIdentifier(Identifier id) { m_id = id; }

  const IdentifierHash& identifierHash() const { return m_id_hash; }

  const Identifier& identify() const { return m_id; }

private:
  IdentifierHash m_id_hash;

  Identifier m_id;
};

#endif // HGTD_RAWDATA_HGTD_RDOCOLLECTION_H
