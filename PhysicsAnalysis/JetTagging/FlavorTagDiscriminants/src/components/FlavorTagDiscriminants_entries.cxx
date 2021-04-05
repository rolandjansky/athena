/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/VRJetOverlapDecoratorTool.h"
#include "FlavorTagDiscriminants/HbbTagTool.h"
#include "FlavorTagDiscriminants/DL2Tool.h"
#include "FlavorTagDiscriminants/BTagAugmenterTool.h"
#include "FlavorTagDiscriminants/BTagMuonAugmenterTool.h"
#include "FlavorTagDiscriminants/BTagJetDecoratorAlg.h"
#include "FlavorTagDiscriminants/BTagToJetLinkerAlg.h"

using namespace FlavorTagDiscriminants;

DECLARE_COMPONENT(VRJetOverlapDecoratorTool)
DECLARE_COMPONENT(HbbTagTool)
DECLARE_COMPONENT(DL2Tool)
DECLARE_COMPONENT(BTagAugmenterTool)
DECLARE_COMPONENT(BTagMuonAugmenterTool)
DECLARE_COMPONENT(BTagJetDecoratorAlg)
DECLARE_COMPONENT(BTagToJetLinkerAlg)
