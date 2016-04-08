// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUTCONTAINER_V1_H
#define XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUTCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigRinger/versions/TrigRNNOutput_v1.h"

namespace xAOD {
   /// Define the RingerRings container as a simple DataVector
   typedef DataVector< TrigRNNOutput_v1 > TrigRNNOutputContainer_v1;
}


#endif // XAODTRIGRINGER_VERSIONS_TRIGRNNOUTPUTCONTAINER_V1_H
