// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRUTH_XAODTRUTHDICT_H
#define XAODTRUTH_XAODTRUTHDICT_H

// Local include(s).
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/versions/TruthParticle_v1.h"
#include "xAODTruth/versions/TruthParticleContainer_v1.h"
#include "xAODTruth/versions/TruthParticleAuxContainer_v1.h"

#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthVertexAuxContainer.h"
#include "xAODTruth/versions/TruthVertex_v1.h"
#include "xAODTruth/versions/TruthVertexContainer_v1.h"
#include "xAODTruth/versions/TruthVertexAuxContainer_v1.h"

#include "xAODTruth/TruthEventBase.h"
#include "xAODTruth/TruthEventBaseContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEventAuxContainer.h"
#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthPileupEventAuxContainer.h"
#include "xAODTruth/versions/TruthEventBase_v1.h"
#include "xAODTruth/versions/TruthEventBaseContainer_v1.h"
#include "xAODTruth/versions/TruthEvent_v1.h"
#include "xAODTruth/versions/TruthEventContainer_v1.h"
#include "xAODTruth/versions/TruthEventAuxContainer_v1.h"
#include "xAODTruth/versions/TruthPileupEvent_v1.h"
#include "xAODTruth/versions/TruthPileupEventContainer_v1.h"
#include "xAODTruth/versions/TruthPileupEventAuxContainer_v1.h"

#include "xAODTruth/TruthMetaData.h"
#include "xAODTruth/TruthMetaDataContainer.h"
#include "xAODTruth/TruthMetaDataAuxContainer.h"
#include "xAODTruth/versions/TruthMetaData_v1.h"
#include "xAODTruth/versions/TruthMetaDataContainer_v1.h"
#include "xAODTruth/versions/TruthMetaDataAuxContainer_v1.h"

#include "xAODTruth/xAODTruthHelpers.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRUTH {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TruthParticleContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TruthVertexContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TruthEventBaseContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TruthEventContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TruthPileupEventContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TruthMetaDataContainer_v1 );
   };
}

#endif // XAODTRUTH_XAODTRUTHDICT_H
