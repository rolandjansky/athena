#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TGC_CondCabling/TGCCablingDbTool.h"

DECLARE_TOOL_FACTORY( TGCCablingDbTool )

DECLARE_FACTORY_ENTRIES( TGC_CondCabling ){
   DECLARE_TOOL( TGCCablingDbTool )
}

