// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGSCONTAINER_V1_H
#define XAODTRIGRINGER_VERSIONS_TRIGRINGERRINGSCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigRinger/versions/TrigRingerRings_v1.h"

namespace xAOD {
   /// Define the TrigRingerRings container as a simple DataVector
   typedef DataVector< TrigRingerRings_v1 > TrigRingerRingsContainer_v1;
}
#endif
