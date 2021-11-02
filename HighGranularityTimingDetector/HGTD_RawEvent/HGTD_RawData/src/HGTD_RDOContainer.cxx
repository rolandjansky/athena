/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RawData/src/HGTD_RDOContainer.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 *
 * @brief Implementation of HGTD_RDOContainer.h
 */

#include "HGTD_RawData/HGTD_RDOContainer.h"

HGTD_RDOContainer::HGTD_RDOContainer(unsigned int hashmax)
    : IdentifiableContainer<HGTD_RDOCollection>(hashmax) {}

const CLID& HGTD_RDOContainer::classID() {
  return ClassID_traits<HGTD_RDOContainer>::ID();
}
