#include "GaudiKernel/DeclareFactoryEntries.h"

#include "JetSelectorTools/JetCleaningTool.h"
#include "JetSelectorTools/EventCleaningTool.h"
#include "../EventCleaningTestAlg.h"

DECLARE_TOOL_FACTORY( JetCleaningTool )
DECLARE_TOOL_FACTORY( ECUtils::EventCleaningTool )

DECLARE_ALGORITHM_FACTORY( EventCleaningTestAlg )

DECLARE_FACTORY_ENTRIES( JetSelectorTools ) 
{
  DECLARE_TOOL( JetCleaningTool );
  DECLARE_TOOL( ECUtils::EventCleaningTool );
  DECLARE_ALGORITHM( EventCleaningTestAlg )
}


