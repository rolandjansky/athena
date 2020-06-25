// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODHIEVENT_XAODHIEVENTDICT_H
#define XAODHIEVENT_XAODHIEVENTDICT_H

// Local include(s):
#include "xAODHIEvent/HIEventShape.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"
#include "xAODHIEvent/versions/HIEventShape_v1.h"
#include "xAODHIEvent/versions/HIEventShapeContainer_v1.h"
#include "xAODHIEvent/versions/HIEventShapeAuxContainer_v1.h"
#include "xAODHIEvent/versions/HIEventShape_v2.h"
#include "xAODHIEvent/versions/HIEventShapeContainer_v2.h"
#include "xAODHIEvent/versions/HIEventShapeAuxContainer_v2.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODHIEVENT {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, HIEventShapeContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, HIEventShapeContainer_v2 );
   };
}

#endif // XAODEVENTINFO_XAODEVENTINFODICT_H
