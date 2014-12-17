// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoIContainer_v2.h 630856 2014-11-25 11:22:49Z gwatts $
#ifndef XAODTRIGGER_VERSIONS_EMTAUCONTAINER_V2_H
#define XAODTRIGGER_VERSIONS_EMTAUCONTAINER_V2_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/EmTauRoI_v2.h"

namespace xAOD {
   /// Declare the em/tau RoI container type
   typedef DataVector< xAOD::EmTauRoI_v2 > EmTauRoIContainer_v2;
}

#endif // XAODTRIGGER_VERSIONS_EMTAUCONTAINER_V2_H
