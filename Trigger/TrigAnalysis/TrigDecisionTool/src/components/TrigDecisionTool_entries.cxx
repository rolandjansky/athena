#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigDecisionTool )

DECLARE_FACTORY_ENTRIES( TrigDecisionTool ) {
   DECLARE_NAMESPACE_TOOL( Trig, TrigDecisionTool )
}

