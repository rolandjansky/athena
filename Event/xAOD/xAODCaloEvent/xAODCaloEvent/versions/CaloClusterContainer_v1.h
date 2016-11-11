// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterContainer_v1.h 783594 2016-11-11 05:03:25Z ssnyder $
#ifndef XAODCALOEVENT_VERSIONS_CALOCLUSTERCONTAINER_V1_H
#define XAODCALOEVENT_VERSIONS_CALOCLUSTERCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODCaloEvent/versions/CaloCluster_v1.h"
#include "xAODCaloEvent/CaloClusterContainerFwd.h"

// Finish declaration of IParticle as a base class of CaloCluster_v1:
DATAVECTOR_BASE_FIN( xAOD::CaloCluster_v1, xAOD::IParticle );

namespace xAOD {
   /// Define the cluster container as a simple DataVector
   typedef DataVector< CaloCluster_v1 > CaloClusterContainer_v1;
}



#endif // XAODCALOEVENT_VERSIONS_CALOCLUSTERCONTAINER_V1_H
