// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARD_VERSIONS_FORWARDEVENTINFOSCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_FORWARDEVENTINFOSCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODForward/versions/ForwardEventInfo_v1.h"

namespace xAOD {
   typedef DataVector< ForwardEventInfo_v1 > ForwardEventInfoContainer_v1;
}

#endif //XAODFORWARD_VERSIONS_FORWARDEVENTINFOSCONTAINER_V1_H
