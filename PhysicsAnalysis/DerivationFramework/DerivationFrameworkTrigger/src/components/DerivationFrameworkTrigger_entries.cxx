#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TriggerMatchingTool.h"

DECLARE_TOOL_FACTORY( DerivationFramework::TriggerMatchingTool )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkTrigger ) {
  DECLARE_TOOL( DerivationFramework::TriggerMatchingTool );
}
