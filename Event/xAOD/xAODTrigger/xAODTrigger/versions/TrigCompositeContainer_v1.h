// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCompositeContainer_v1.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGCOMPOSITECONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_TRIGCOMPOSITECONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/TrigComposite_v1.h"

namespace xAOD {
   /// The container is a simple typedef
   typedef DataVector< TrigComposite_v1 > TrigCompositeContainer_v1;
}

#endif // XAODTRIGGER_VERSIONS_TRIGCOMPOSITECONTAINER_V1_H
