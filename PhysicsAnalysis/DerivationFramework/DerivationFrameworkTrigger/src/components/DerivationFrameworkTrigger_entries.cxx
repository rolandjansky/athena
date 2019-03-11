#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TriggerMatchingTool.h"
#include "../TriggerMatchingTestAlg.h"

DECLARE_TOOL_FACTORY( DerivationFramework::TriggerMatchingTool )
DECLARE_ALGORITHM_FACTORY( TriggerMatchingTestAlg )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkTrigger ) {
  DECLARE_TOOL( DerivationFramework::TriggerMatchingTool );
  DECLARE_ALGORITHM( TriggerMatchingTestAlg )
}
