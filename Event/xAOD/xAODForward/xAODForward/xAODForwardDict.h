// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODFORWARD_XAODFORWARDDICT_H
#define XAODFORWARD_XAODFORWARDDICT_H

// Local include(s).
#include "xAODForward/ALFADataContainer.h"
#include "xAODForward/ALFADataAuxContainer.h"
#include "xAODForward/versions/ALFADataContainer_v1.h"
#include "xAODForward/versions/ALFADataAuxContainer_v1.h"
#include "xAODForward/versions/ALFAData_v1.h"

#include "xAODForward/AFPDataContainer.h"
#include "xAODForward/AFPDataAuxContainer.h"
#include "xAODForward/versions/AFPDataContainer_v1.h"
#include "xAODForward/versions/AFPDataAuxContainer_v1.h"
#include "xAODForward/versions/AFPData_v1.h"

#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFHitAuxContainer.h"
#include "xAODForward/versions/AFPToFHitContainer_v1.h"
#include "xAODForward/versions/AFPToFHitAuxContainer_v1.h"
#include "xAODForward/versions/AFPToFHit_v1.h"

#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitAuxContainer.h"
#include "xAODForward/versions/AFPSiHitContainer_v1.h"
#include "xAODForward/versions/AFPSiHitAuxContainer_v1.h"
#include "xAODForward/versions/AFPSiHit_v1.h"
#include "xAODForward/versions/AFPSiHitContainer_v2.h"
#include "xAODForward/versions/AFPSiHitAuxContainer_v2.h"
#include "xAODForward/versions/AFPSiHit_v2.h"

#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPSiHitsClusterAuxContainer.h"
#include "xAODForward/versions/AFPSiHitsClusterContainer_v1.h"
#include "xAODForward/versions/AFPSiHitsClusterAuxContainer_v1.h"
#include "xAODForward/versions/AFPSiHitsCluster_v1.h"

#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"
#include "xAODForward/versions/AFPTrackContainer_v1.h"
#include "xAODForward/versions/AFPTrackAuxContainer_v1.h"
#include "xAODForward/versions/AFPTrack_v1.h"
#include "xAODForward/versions/AFPTrackContainer_v2.h"
#include "xAODForward/versions/AFPTrackAuxContainer_v2.h"
#include "xAODForward/versions/AFPTrack_v2.h"

#include "xAODForward/AFPProtonContainer.h"
#include "xAODForward/AFPProtonAuxContainer.h"
#include "xAODForward/versions/AFPProtonContainer_v1.h"
#include "xAODForward/versions/AFPProtonAuxContainer_v1.h"
#include "xAODForward/versions/AFPProton_v1.h"

#include "xAODForward/ZdcModuleContainer.h"
#include "xAODForward/ZdcModuleAuxContainer.h"
#include "xAODForward/versions/ZdcModuleContainer_v1.h"
#include "xAODForward/versions/ZdcModuleAuxContainer_v1.h"
#include "xAODForward/versions/ZdcModule_v1.h"

#include "xAODForward/MBTSModuleContainer.h"
#include "xAODForward/MBTSModuleAuxContainer.h"
#include "xAODForward/versions/MBTSModuleContainer_v1.h"
#include "xAODForward/versions/MBTSModuleAuxContainer_v1.h"
#include "xAODForward/versions/MBTSModule_v1.h"

#include "xAODForward/ForwardEventInfoContainer.h"
#include "xAODForward/ForwardEventInfoAuxContainer.h"
#include "xAODForward/versions/ForwardEventInfoContainer_v1.h"
#include "xAODForward/versions/ForwardEventInfoAuxContainer_v1.h"
#include "xAODForward/versions/ForwardEventInfo_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODFORWARD {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, AFPDataContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, AFPToFHitContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, AFPSiHitContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, AFPSiHitContainer_v2 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, AFPSiHitsClusterContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, AFPTrackContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, AFPTrackContainer_v2 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, AFPProtonContainer_v1 );

      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, ALFADataContainer_v1 );

      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, ZdcModuleContainer_v1 );

      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, MBTSModuleContainer_v1 );

      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, ForwardEventInfoContainer_v1 );

      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TriggerTowerContainer );
   };
}

#endif // XAODFORWARD_XAODFORWARDDICT_H
