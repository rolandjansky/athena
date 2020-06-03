// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODEVENTINFO_XAODEVENTINFODICT_H
#define XAODEVENTINFO_XAODEVENTINFODICT_H

// Local include(s).
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "xAODEventInfo/versions/EventInfo_v1.h"
#include "xAODEventInfo/versions/EventAuxInfo_v1.h"
#include "xAODEventInfo/versions/EventAuxInfo_v2.h"
#include "xAODEventInfo/versions/EventInfoContainer_v1.h"
#include "xAODEventInfo/versions/EventInfoAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODEVENTINFO {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, EventInfoContainer_v1 );
      XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, EventInfo_v1 );
   };
} // private namespace

#endif // XAODEVENTINFO_XAODEVENTINFODICT_H
