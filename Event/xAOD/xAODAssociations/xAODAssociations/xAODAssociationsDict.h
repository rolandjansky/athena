// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODASSOCIATIONS_XAODASSOCIATIONSDICT_H
#define XAODASSOCIATIONS_XAODASSOCIATIONSDICT_H
 
// Local include(s).
#include "xAODAssociations/TrackParticleClusterAssociation.h"
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODAssociations/TrackParticleClusterAssociationAuxContainer.h"
#include "xAODAssociations/versions/TrackParticleClusterAssociation_v1.h"
#include "xAODAssociations/versions/TrackParticleClusterAssociationContainer_v1.h"
#include "xAODAssociations/versions/TrackParticleClusterAssociationAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODASSOCIATIONS {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD,
                                  TrackParticleClusterAssociationContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrackParticleContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CaloClusterContainer );
   };
}

#endif // XAODASSOCIATIONS_XAODASSOCIATIONSDICT_H
