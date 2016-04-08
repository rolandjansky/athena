// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local includes
#include "../TrigCostD3PDMakerTool.h"
#include "../TrigConfMetadataTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigCostD3PDMakerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigConfMetadataTool )

DECLARE_FACTORY_ENTRIES( TrigCostD3PDMaker ) {
  DECLARE_NAMESPACE_TOOL( D3PD, TrigCostD3PDMakerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, TrigCostD3PDMakerTool )

}
