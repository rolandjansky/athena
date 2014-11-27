// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterContainer_v1.h 631117 2014-11-26 08:48:22Z gwatts $
#ifndef XAODTRIGCALO_VERSIONS_TRIGEMCLUSTERCONTAINER_V1_H
#define XAODTRIGCALO_VERSIONS_TRIGEMCLUSTERCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigCalo/versions/TrigEMCluster_v1.h"

namespace xAOD {
   /// Define the trigger EM cluster container as a simple DataVector
   typedef DataVector< TrigEMCluster_v1 > TrigEMClusterContainer_v1;
}

#endif // XAODTRIGCALO_VERSIONS_TRIGEMCLUSTERCONTAINER_V1_H
