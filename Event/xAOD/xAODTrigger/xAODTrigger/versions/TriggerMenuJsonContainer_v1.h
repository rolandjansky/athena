// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_VERSIONS_MENUJSONCONTAINER_v1_H
#define XAODTRIGGER_VERSIONS_MENUJSONCONTAINER_v1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuJson_v1.h"

namespace xAOD {
   /// Define the menu container as a simple DataVector
   typedef DataVector< TriggerMenuJson_v1 > TriggerMenuJsonContainer_v1;
}

#endif // XAODTRIGGER_VERSIONS_MENUJSONCONTAINER_v1_H
