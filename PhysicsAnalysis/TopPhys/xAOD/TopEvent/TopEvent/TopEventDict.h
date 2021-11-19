// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef TOPEVENT_TOPEVENTDICT_H
#define TOPEVENT_TOPEVENTDICT_H

// Local include(s).
#include "TopEvent/KLFitterResult.h"
#include "TopEvent/KLFitterResultContainer.h"
#include "TopEvent/KLFitterResultAuxContainer.h"

#include "TopEvent/PseudoTopResult.h"
#include "TopEvent/PseudoTopResultContainer.h"

#include "TopEvent/SystematicEvent.h"
#include "TopEvent/SystematicEventContainer.h"
#include "TopEvent/SystematicEventAuxContainer.h"

#include "TopEvent/RCJet.h"
#include "TopEvent/TopEventMaker.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_TOPEVENT {
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, KLFitterResultContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, PseudoTopResultContainer );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, SystematicEventContainer );
   };
}

#endif // not TOPEVENT_TOPEVENTDICT_H
