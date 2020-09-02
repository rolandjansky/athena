// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCALOEVENT_VERSIONS_CALOCLUSTERCONTAINER_V1_H
#define XAODCALOEVENT_VERSIONS_CALOCLUSTERCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"
// Local include(s):
#include "xAODCaloEvent/versions/CaloCluster_v1.h"

namespace xAOD {
   /// Define the cluster container as a simple DataVector
   typedef DataVector< CaloCluster_v1 > CaloClusterContainer_v1;
}

#endif // XAODCALOEVENT_VERSIONS_CALOCLUSTERCONTAINER_V1_H
