/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RawData/HGTD_RDO_Container.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 *
 * @brief
 */

#ifndef HGTD_RAWDATA_HGTD_RDO_CONTAINER_H
#define HGTD_RAWDATA_HGTD_RDO_CONTAINER_H

#include "EventContainers/IdentifiableContainer.h"
#include "HGTD_RawData/HGTD_RDO_Collection.h"
#include "AthenaKernel/CLASS_DEF.h"

class HGTD_RDO_Container
    : public IdentifiableContainer<HGTD_RDO_Collection> {
  // friend class HGTD_RDO_CollectionCnv_p1; //FIXME probably later

public:
  /**
   * @brief Default constructor should NOT be used.
   */
  HGTD_RDO_Container() = delete;
  ~HGTD_RDO_Container() = default;

  HGTD_RDO_Container(unsigned int hashmax);

  static const CLID& classID();

  virtual const CLID& clID() const { return classID(); }
};

CLASS_DEF(HGTD_RDO_Container, 1218198717, 1)

#endif // HGTD_RAWDATA_HGTD_RDO_CONTAINER_H
