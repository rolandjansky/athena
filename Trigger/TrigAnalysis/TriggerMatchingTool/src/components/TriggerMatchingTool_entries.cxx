#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TriggerMatchingTool/MatchingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trig, MatchingTool )



#include "../TestMatchingToolAlg.h"
DECLARE_ALGORITHM_FACTORY( TestMatchingToolAlg )



DECLARE_FACTORY_ENTRIES( TriggerMatchingTool ) {
  DECLARE_ALGORITHM( TestMatchingToolAlg );
  DECLARE_NAMESPACE_TOOL(Trig, MatchingTool)
}
