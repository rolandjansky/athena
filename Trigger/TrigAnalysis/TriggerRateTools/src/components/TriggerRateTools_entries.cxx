#include "TriggerRateTools/TriggerRateTools.h"
#include "TriggerRateTools/EvtVetoTool.h"
#include "TriggerRateTools/SlicesInfoTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TriggerRateTools )
DECLARE_TOOL_FACTORY( EvtVetoTool )
DECLARE_TOOL_FACTORY( SlicesInfoTool )
        
DECLARE_FACTORY_ENTRIES( TriggerRateTools ) {
  DECLARE_ALGORITHM( TriggerRateTools )
  DECLARE_TOOL( EvtVetoTool )
  DECLARE_TOOL( SlicesInfoTool )
}

