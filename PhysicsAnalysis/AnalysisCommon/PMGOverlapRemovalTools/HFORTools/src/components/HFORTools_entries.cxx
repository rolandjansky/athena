
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "HFORTools/HFORSelectionTool.h"
#include "../HFORSelectionAlg.h"

DECLARE_ALGORITHM_FACTORY( HFORSelectionAlg )

DECLARE_TOOL_FACTORY( HFORSelectionTool )

DECLARE_FACTORY_ENTRIES( HFORTools ) 
{
  DECLARE_ALGORITHM( HFORSelectionAlg );
  DECLARE_TOOL( HFORSelectionTool );
}
