#include "GaudiKernel/DeclareFactoryEntries.h"

// Tool
#include "../TBCaloPosTool.h" 
#include "../TBCaloCoolPosTool.h" 
DECLARE_TOOL_FACTORY( TBCaloPosTool ) 
DECLARE_TOOL_FACTORY( TBCaloCoolPosTool ) 

DECLARE_FACTORY_ENTRIES(TBCaloConditions) {
  DECLARE_TOOL ( TBCaloPosTool )
  DECLARE_TOOL ( TBCaloCoolPosTool )
}

