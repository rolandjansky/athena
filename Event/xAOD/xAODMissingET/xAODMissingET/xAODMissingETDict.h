// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODMISSINGET_XAODMISSINGETDICT_H
#define XAODMISSINGET_XAODMISSINGETDICT_H

// Local include(s).
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/versions/MissingET_v1.h"
#include "xAODMissingET/versions/MissingETContainer_v1.h"
#include "xAODMissingET/versions/MissingETAuxContainer_v1.h"

#include "xAODMissingET/MissingETComponent.h"
#include "xAODMissingET/MissingETComponentMap.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"
#include "xAODMissingET/versions/MissingETComponent_v1.h"
#include "xAODMissingET/versions/MissingETComponentMap_v1.h"
#include "xAODMissingET/versions/MissingETAuxComponentMap_v1.h"

#include "xAODMissingET/MissingETAssociation.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETAuxAssociationMap.h"
#include "xAODMissingET/versions/MissingETAssociation_v1.h"
#include "xAODMissingET/versions/MissingETAssociationMap_v1.h"
#include "xAODMissingET/versions/MissingETAuxAssociationMap_v1.h"
#include "xAODMissingET/versions/MissingETAuxAssociationMap_v2.h"

#include "xAODMissingET/versions/MissingETBase.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODMISSINGET {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, MissingETContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, MissingETComponentMap_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, MissingETAssociationMap_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, IParticleContainer );
   };
}

#endif // XAODMISSINGET_XAODMISSINGETDICT_H
