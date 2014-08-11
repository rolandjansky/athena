// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoIContainer_v1.h 574653 2013-12-09 15:53:17Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_JETROICONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_JETROICONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/JetRoI_v1.h"

namespace xAOD {
   /// Declare the jet RoI container type
   typedef DataVector< xAOD::JetRoI_v1 > JetRoIContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::JetRoIContainer_v1, 1132837218, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_VERSIONS_JETROICONTAINER_V1_H
