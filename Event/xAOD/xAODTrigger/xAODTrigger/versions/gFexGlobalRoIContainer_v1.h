// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_GFEXGLOBALROICONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_GFEXGLOBALROICONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/gFexGlobalRoI_v1.h"

namespace xAOD {
   /// Declare the Global TOB container type
   typedef DataVector< xAOD::gFexGlobalRoI_v1 > gFexGlobalRoIContainer_v1;
}

#endif // XAODTRIGGER_VERSIONS_GFEXGLOBALROICONTAINER_V1_H
