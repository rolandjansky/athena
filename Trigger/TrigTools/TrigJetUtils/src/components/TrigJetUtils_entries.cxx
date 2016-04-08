#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../JKDTrigHelperTool.h"
#include "../TrigJetFeaturesUnpacker.h"

DECLARE_TOOL_FACTORY (JKDTrigHelperTool)
DECLARE_ALGORITHM_FACTORY (TrigJetFeaturesUnpacker)

DECLARE_FACTORY_ENTRIES( TrigJetUtils ) {
  DECLARE_TOOL(JKDTrigHelperTool)
  DECLARE_ALGORITHM(TrigJetFeaturesUnpacker)
}


