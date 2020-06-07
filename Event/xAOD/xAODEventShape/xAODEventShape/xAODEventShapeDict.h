// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODEVENTSHAPE_XAODEVENTSHAPEDICT_H
#define XAODEVENTSHAPE_XAODEVENTSHAPEDICT_H

// Local include(s).
#include "xAODEventShape/EventShape.h"
#include "xAODEventShape/EventShapeAuxInfo.h"
#include "xAODEventShape/versions/EventShape_v1.h"
#include "xAODEventShape/versions/EventShapeAuxInfo_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODEVENTSHAPE {
      // Local type(s).
      XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, EventShape_v1 );
   };
}

#endif // XAODEVENTSHAPE_XAODEVENTSHAPEDICT_H
