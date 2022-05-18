/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RawData/src/HGTD_RDO_Container.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>

 *
 * @brief Implementation of HGTD_RDO_Container.h
 */

#include "HGTD_RawData/HGTD_RDO_Container.h"

HGTD_RDO_Container::HGTD_RDO_Container(unsigned int hashmax)
  : IdentifiableContainer<HGTD_RDO_Collection>(hashmax) {}

const CLID& HGTD_RDO_Container::classID() {
  return ClassID_traits<HGTD_RDO_Container>::ID();
}
