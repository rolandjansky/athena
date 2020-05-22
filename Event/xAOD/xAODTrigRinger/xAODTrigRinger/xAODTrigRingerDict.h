// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGRINGER_XAODTRIGRINGERDICT_H
#define XAODTRIGRINGER_XAODTRIGRINGERDICT_H

// Local include(s).
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"
#include "xAODTrigRinger/versions/TrigRingerRings_v1.h"
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRingerRings_v2.h"
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v2.h"
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v2.h"

#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRNNOutputAuxContainer.h"
#include "xAODTrigRinger/versions/TrigRNNOutput_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v1.h"
#include "xAODTrigRinger/versions/TrigRNNOutput_v2.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v2.h"
#include "xAODTrigRinger/versions/TrigRNNOutputAuxContainer_v2.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODRINGERRINGS {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigRingerRingsContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigRingerRingsContainer_v2 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigRNNOutputContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigRNNOutputContainer_v2 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigEMClusterContainer );
   };
}

#endif // XAODTRIGRINGER_XAODTRIGRINGERDICT_H
