#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../CpmMappingTool.h"
#include "../JemMappingTool.h"
#include "../PpmCoolMappingTool.h"
#include "../PpmCoolOrBuiltinMappingTool.h"
#include "../PpmMappingTool.h"


// declare 
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, CpmMappingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, JemMappingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, PpmMappingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, PpmCoolMappingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, PpmCoolOrBuiltinMappingTool )

DECLARE_FACTORY_ENTRIES( TrigT1CaloMappingTools )
{
  DECLARE_NAMESPACE_TOOL( LVL1, CpmMappingTool )
  DECLARE_NAMESPACE_TOOL( LVL1, JemMappingTool )
  DECLARE_NAMESPACE_TOOL( LVL1, PpmMappingTool )
  DECLARE_NAMESPACE_TOOL( LVL1, PpmCoolMappingTool )
  DECLARE_NAMESPACE_TOOL( LVL1, PpmCoolOrBuiltinMappingTool )
}
