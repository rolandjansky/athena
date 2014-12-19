// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterContainer_v1.h 628097 2014-11-13 09:24:54Z krasznaa $
#ifndef XAODCALOEVENT_VERSIONS_CALOCLUSTERCONTAINER_V1_H
#define XAODCALOEVENT_VERSIONS_CALOCLUSTERCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODCaloEvent/versions/CaloCluster_v1.h"

// Declare IParticle as a base class of CaloCluster_v1:
DATAVECTOR_BASE( xAOD::CaloCluster_v1, xAOD::IParticle );

namespace xAOD {
   /// Define the cluster container as a simple DataVector
   typedef DataVector< CaloCluster_v1 > CaloClusterContainer_v1;
}



#endif // XAODCALOEVENT_VERSIONS_CALOCLUSTERCONTAINER_V1_H
