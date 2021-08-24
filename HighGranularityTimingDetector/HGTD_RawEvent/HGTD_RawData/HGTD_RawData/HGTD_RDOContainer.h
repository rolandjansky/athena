/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RawData/HGTD_RDOContainer.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief
 */

#ifndef HGTD_RAWDATA_HGTD_RDOCONTAINER_H
#define HGTD_RAWDATA_HGTD_RDOCONTAINER_H

#include "EventContainers/IdentifiableContainer.h"
#include "HGTD_RawData/HGTD_RDOCollection.h"
#include "SGTools/CLASS_DEF.h"

namespace HGTD {

class HGTD_RDOContainer
    : public IdentifiableContainer<HGTD::HGTD_RDOCollection> {
  // friend class HGTD_RDOCollectionCnv_p1; //FIXME probably later

public:
  /**
   * @brief Default constructor should NOT be used, but is needed for pool I/O.
   */
  HGTD_RDOContainer() = default;
  ~HGTD_RDOContainer() = default;

  HGTD_RDOContainer(unsigned int hashmax);

  static const CLID& classID();

  virtual const CLID& clID() const { return classID(); }
};

} // namespace HGTD

CLASS_DEF(HGTD::HGTD_RDOContainer, 1145604774, 1)

#endif // HGTD_RAWDATA_HGTD_RDOCONTAINER_H
