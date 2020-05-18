/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "FlavorTagDiscriminants/VRJetOverlapDecoratorTool.h"
#include "FlavorTagDiscriminants/HbbTagTool.h"
#include "FlavorTagDiscriminants/DL2Tool.h"
#include "FlavorTagDiscriminants/BTagAugmenterTool.h"
#include "FlavorTagDiscriminants/BTagMuonAugmenterTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY(FlavorTagDiscriminants, VRJetOverlapDecoratorTool)
DECLARE_NAMESPACE_TOOL_FACTORY(FlavorTagDiscriminants, HbbTagTool)
DECLARE_NAMESPACE_TOOL_FACTORY(FlavorTagDiscriminants, DL2Tool)
DECLARE_NAMESPACE_TOOL_FACTORY(FlavorTagDiscriminants, BTagAugmenterTool)
DECLARE_NAMESPACE_TOOL_FACTORY(FlavorTagDiscriminants, BTagMuonAugmenterTool)

DECLARE_FACTORY_ENTRIES(FlavorTagDiscriminants) {
  DECLARE_NAMESPACE_TOOL(FlavorTagDiscriminants, VRJetOverlapDecoratorTool)
  DECLARE_NAMESPACE_TOOL(FlavorTagDiscriminants, HbbTagTool)
  DECLARE_NAMESPACE_TOOL(FlavorTagDiscriminants, DL2Tool)
  DECLARE_NAMESPACE_TOOL(FlavorTagDiscriminants, BTagAugmenterTool)
  DECLARE_NAMESPACE_TOOL(FlavorTagDiscriminants, BTagMuonAugmenterTool)
}
