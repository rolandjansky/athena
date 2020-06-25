// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALOEVENT_XAODCALOEVENTDICT_H
#define XAODCALOEVENT_XAODCALOEVENTDICT_H

// Local include(s):
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/versions/CaloCluster_v1.h"
#include "xAODCaloEvent/versions/CaloClusterContainer_v1.h"
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v2.h"

#include "xAODCaloEvent/CaloTower.h"
#include "xAODCaloEvent/CaloTowerContainer.h"
#include "xAODCaloEvent/CaloTowerAuxContainer.h"
#include "xAODCaloEvent/versions/CaloTower_v1.h"
#include "xAODCaloEvent/versions/CaloTowerContainer_v1.h"
#include "xAODCaloEvent/versions/CaloTowerAuxContainer_v1.h"

#include "xAODCaloEvent/CaloClusterBadChannelData.h"
#include "xAODCaloEvent/versions/CaloClusterBadChannelData_v1.h"

#include "xAODCaloEvent/CaloVertexedClusterBase.h"
#include "xAODCaloEvent/CaloVertexedTopoCluster.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODCALOEVENT {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CaloClusterContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CaloTowerContainer_v1 );
      std::vector< xAOD::CaloClusterBadChannelData_v1 > v1;
      std::vector< std::vector< xAOD::CaloClusterBadChannelData_v1 > > v2;
   };
}

#endif // XAODCALOEVENT_XAODCALOEVENTDICT_H
