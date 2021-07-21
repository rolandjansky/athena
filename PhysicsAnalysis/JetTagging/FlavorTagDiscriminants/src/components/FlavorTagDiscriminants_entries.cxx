/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/VRJetOverlapDecoratorTool.h"
#include "FlavorTagDiscriminants/HbbTagTool.h"
#include "FlavorTagDiscriminants/DL2Tool.h"
#include "FlavorTagDiscriminants/BTagAugmenterTool.h"
#include "FlavorTagDiscriminants/BTagMuonAugmenterTool.h"
#include "FlavorTagDiscriminants/BTagDecoratorAlg.h"
#include "FlavorTagDiscriminants/BTagToJetLinkerAlg.h"
#include "FlavorTagDiscriminants/JetToBTagLinkerAlg.h"
#include "FlavorTagDiscriminants/BTagTrackLinkCopyAlg.h"
#include "FlavorTagDiscriminants/BTaggingBuilderAlg.h"

using namespace FlavorTagDiscriminants;

DECLARE_COMPONENT(VRJetOverlapDecoratorTool)
DECLARE_COMPONENT(HbbTagTool)
DECLARE_COMPONENT(DL2Tool)
DECLARE_COMPONENT(BTagAugmenterTool)
DECLARE_COMPONENT(BTagMuonAugmenterTool)
DECLARE_COMPONENT(BTagDecoratorAlg)
DECLARE_COMPONENT(BTagToJetLinkerAlg)
DECLARE_COMPONENT(JetToBTagLinkerAlg)
DECLARE_COMPONENT(BTagTrackLinkCopyAlg)
DECLARE_COMPONENT(BTaggingBuilderAlg)
