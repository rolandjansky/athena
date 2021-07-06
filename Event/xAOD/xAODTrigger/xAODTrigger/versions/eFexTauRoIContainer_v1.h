// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_EFEXTAUROICONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_EFEXTAUROICONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/eFexTauRoI_v1.h"

namespace xAOD {
   /// Declare the tau RoI container type
   typedef DataVector< xAOD::eFexTauRoI_v1 > eFexTauRoIContainer_v1;
}

#endif // XAODTRIGGER_VERSIONS_EFEXTAUROICONTAINER_V1_H
