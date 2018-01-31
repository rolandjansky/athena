#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h"

DECLARE_TOOL_FACTORY( TrigGlobalEfficiencyCorrectionTool )

DECLARE_FACTORY_ENTRIES( TrigGlobalEfficiencyCorrection ) 
{
  DECLARE_TOOL( TrigGlobalEfficiencyCorrectionTool );
}