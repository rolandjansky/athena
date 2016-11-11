// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODCALOEVENT_CALOCLUSTERCONTAINERFWD_V1_H
#define XAODCALOEVENT_CALOCLUSTERCONTAINERFWD_V1_H

#include "AthContainers/DataVector.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODBase/IParticleContainer.h"

DATAVECTOR_BASE_FWD( xAOD::CaloCluster_v1, xAOD::IParticle );

namespace xAOD {
   /// Define the cluster container as a simple DataVector
   typedef DataVector< CaloCluster_v1 > CaloClusterContainer_v1;
}


#endif // XAODCALOEVENT_CALOCLUSTERCONTAINERFWD_V1_H
