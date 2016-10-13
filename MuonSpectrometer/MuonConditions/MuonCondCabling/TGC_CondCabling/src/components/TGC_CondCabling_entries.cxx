#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TGC_CondCabling/TGCCablingDbTool.h"
#include "TGC_CondCabling/TGCTriggerDbTool.h"

DECLARE_TOOL_FACTORY( TGCCablingDbTool )
DECLARE_TOOL_FACTORY( TGCTriggerDbTool )

DECLARE_FACTORY_ENTRIES( TGC_CondCabling ){
   DECLARE_TOOL( TGCCablingDbTool )
   DECLARE_TOOL( TGCTriggerDbTool )
}

