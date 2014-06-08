// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBphysContainer_v1.h 600124 2014-06-03 15:35:20Z krasznaa $
#ifndef XAODTRIGBPHYS_VERSIONS_TRIGBPHYSCONTAINER_V1_H
#define XAODTRIGBPHYS_VERSIONS_TRIGBPHYSCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigBphys/versions/TrigBphys_v1.h"

namespace xAOD {
    
    /// Declare the em/tau RoI container type
    typedef DataVector< xAOD::TrigBphys_v1 > TrigBphysContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigBphysContainer_v1, 1329567956, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTRIGBPHYS_VERSIONS_TRIGBPHYSCONTAINER_V1_H
