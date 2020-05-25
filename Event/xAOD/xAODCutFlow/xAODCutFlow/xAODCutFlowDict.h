// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCUTFLOW_XAODCUTFLOWDICT_H
#define XAODCUTFLOW_XAODCUTFLOWDICT_H

// Local include(s).
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"
#include "xAODCutFlow/versions/CutBookkeeper_v1.h"
#include "xAODCutFlow/versions/CutBookkeeperContainer_v1.h"
#include "xAODCutFlow/versions/CutBookkeeperAuxContainer_v1.h"

#include "xAODCutFlow/xAODCutFlowHelpers.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODCUTFLOW {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CutBookkeeperContainer_v1 );
   };
}

#endif // XAODCUTFLOW_XAODCUTFLOWDICT_H
