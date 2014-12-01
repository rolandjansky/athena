// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBphysContainer_v1.h 631136 2014-11-26 11:22:46Z gwatts $
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

#endif // XAODTRIGBPHYS_VERSIONS_TRIGBPHYSCONTAINER_V1_H
