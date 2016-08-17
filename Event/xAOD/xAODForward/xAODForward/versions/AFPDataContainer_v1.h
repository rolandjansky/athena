// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AFPDataContainer_v1.h 693858 2015-09-09 10:30:15Z lnozka $
#ifndef XAODFORWARD_VERSIONS_AFPDATACONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPDATACONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODForward/versions/AFPData_v1.h"

namespace xAOD
{
   typedef DataVector< AFPData_v1 > AFPDataContainer_v1;
}

#endif //XAODFORWARD_VERSIONS_AFPDATACONTAINER_V1_H
