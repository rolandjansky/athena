// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCALOEVENT_CALOCLUSTERCONTAINERFWD_V1_H
#define XAODCALOEVENT_CALOCLUSTERCONTAINERFWD_V1_H

#include "AthContainers/DataVector.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODBase/IParticleContainer.h"

DATAVECTOR_BASE_FWD( xAOD::CaloCluster, xAOD::IParticle );

namespace xAOD {
   /// Define the cluster container as a simple DataVector
   typedef DataVector< CaloCluster > CaloClusterContainer;
}


#endif // XAODCALOEVENT_CALOCLUSTERCONTAINERFWD_V1_H
