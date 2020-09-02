// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_XAODCALORINGSDICT_H
#define XAODCALORINGS_XAODCALORINGSDICT_H

// Local include(s):
#include "xAODCaloRings/tools/AtlasGeoPoint.h"

#include "xAODCaloRings/RingSet.h"
#include "xAODCaloRings/RingSetContainer.h"
#include "xAODCaloRings/RingSetAuxContainer.h"
#include "xAODCaloRings/versions/RingSet_v1.h"
#include "xAODCaloRings/versions/RingSetContainer_v1.h"
#include "xAODCaloRings/versions/RingSetAuxContainer_v1.h"

#include "xAODCaloRings/RingSetConf.h"
#include "xAODCaloRings/RingSetConfContainer.h"
#include "xAODCaloRings/RingSetConfAuxContainer.h"
#include "xAODCaloRings/versions/RingSetConf_v1.h"
#include "xAODCaloRings/versions/RingSetConfContainer_v1.h"
#include "xAODCaloRings/versions/RingSetConfAuxContainer_v1.h"

#include "xAODCaloRings/CaloRings.h"
#include "xAODCaloRings/CaloRingsContainer.h"
#include "xAODCaloRings/CaloRingsAuxContainer.h"
#include "xAODCaloRings/versions/CaloRings_v1.h"
#include "xAODCaloRings/versions/CaloRingsContainer_v1.h"
#include "xAODCaloRings/versions/CaloRingsAuxContainer_v1.h"

#include "xAODCaloRings/CaloRingsDefs.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODCALORINGS {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, RingSetContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, RingSetConfContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CaloRingsContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      std::vector< CaloSampling::CaloSample > sample_vec;
   };
} // private namespace

#endif // XAODCALORINGS_XAODCALORINGSDICT_H
