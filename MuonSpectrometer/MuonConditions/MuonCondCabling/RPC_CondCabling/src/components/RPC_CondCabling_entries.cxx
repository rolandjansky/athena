#include "GaudiKernel/DeclareFactoryEntries.h"
#include "RPC_CondCabling/RPCTriggerDbTool.h"
#include "RPC_CondCabling/RPCCablingDbTool.h"

DECLARE_TOOL_FACTORY( RPCCablingDbTool )
DECLARE_TOOL_FACTORY( RPCTriggerDbTool )


DECLARE_FACTORY_ENTRIES( RPC_CondCabling ){
   DECLARE_TOOL( RPCCablingDbTool )
   DECLARE_TOOL( RPCTriggerDbTool )
}

