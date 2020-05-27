// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGCALO_XAODTRIGCALODICT_H
#define XAODTRIGCALO_XAODTRIGCALODICT_H

// Local include(s):
#include "xAODTrigCalo/TrigCaloCluster.h"
#include "xAODTrigCalo/TrigCaloClusterContainer.h"
#include "xAODTrigCalo/TrigCaloClusterAuxContainer.h"
#include "xAODTrigCalo/versions/TrigCaloCluster_v1.h"
#include "xAODTrigCalo/versions/TrigCaloClusterContainer_v1.h"
#include "xAODTrigCalo/versions/TrigCaloClusterAuxContainer_v1.h"

#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigCalo/versions/TrigEMCluster_v1.h"
#include "xAODTrigCalo/versions/TrigEMClusterContainer_v1.h"
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v1.h"
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v2.h"

#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"
#include "xAODTrigCalo/versions/CaloClusterTrigAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGCALO {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigCaloClusterContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigEMClusterContainer_v1 );
   };
}

#endif // XAODTRIGCALO_XAODTRIGCALODICT_H
