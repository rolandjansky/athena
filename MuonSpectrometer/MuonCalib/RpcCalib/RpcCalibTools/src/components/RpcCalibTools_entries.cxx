#include "GaudiKernel/DeclareFactoryEntries.h"

#include "RpcCalibTools/RpcExtrapolationTool.h"
#include "RpcCalibTools/RpcResidualsTool.h"

DECLARE_TOOL_FACTORY(RpcExtrapolationTool)
DECLARE_TOOL_FACTORY(RpcResidualsTool)

DECLARE_FACTORY_ENTRIES(RpcCalibTools) {
  DECLARE_TOOL( RpcExtrapolationTool )
  DECLARE_TOOL( RpcResidualsTool )
}

