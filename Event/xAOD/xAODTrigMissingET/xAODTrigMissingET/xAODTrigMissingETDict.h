// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef xAODTrigMissingET_xAODTrigMissingET_DICT_H
#define xAODTrigMissingET_xAODTrigMissingET_DICT_H

// Local include(s):
#include "xAODTrigMissingET/TrigMissingET.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"
#include "xAODTrigMissingET/versions/TrigMissingET_v1.h"
#include "xAODTrigMissingET/versions/TrigMissingETContainer_v1.h"
#include "xAODTrigMissingET/versions/TrigMissingETAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGMISSINGET {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigMissingETContainer_v1 );
   };
}

#endif // xAODTrigMissingET_xAODTrigMissingET_DICT_H
