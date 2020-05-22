// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGBPHYS_XAODTRIGBPHYSDICT_H
#define XAODTRIGBPHYS_XAODTRIGBPHYSDICT_H

// Local include(s).
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"
#include "xAODTrigBphys/versions/TrigBphys_v1.h"
#include "xAODTrigBphys/versions/TrigBphysContainer_v1.h"
#include "xAODTrigBphys/versions/TrigBphysAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace{
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGBPHYS {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigBphysContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrackParticleContainer );
   };
}

#endif // XAODTRIGBPHYS_XAODTRIGBPHYSDICT_H
