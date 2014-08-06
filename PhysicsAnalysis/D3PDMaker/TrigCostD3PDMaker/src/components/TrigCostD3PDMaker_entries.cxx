// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local includes
#include "../TrigCostBaseFillerTool.h"
#include "../TrigCostChainFillerTool.h"
#include "../TrigCostL1FillerTool.h"
#include "../TrigCostROBFillerTool.h"
#include "../TrigCostRoIFillerTool.h"
#include "../TrigCostSequenceFillerTool.h"
#include "../TrigCostTrigElementFillerTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigCostBaseFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigCostChainFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigCostL1FillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigCostROBFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigCostRoIFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigCostSequenceFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, TrigCostTrigElementFillerTool )

DECLARE_FACTORY_ENTRIES( TrigCostD3PDMaker ) {
	DECLARE_NAMESPACE_TOOL( D3PD, TrigCostBaseFillerTool )
	DECLARE_NAMESPACE_TOOL( D3PD, TrigCostChainFillerTool )
	DECLARE_NAMESPACE_TOOL( D3PD, TrigCostL1FillerTool )
	DECLARE_NAMESPACE_TOOL( D3PD, TrigCostROBFillerTool )
	DECLARE_NAMESPACE_TOOL( D3PD, TrigCostRoIFillerTool )
	DECLARE_NAMESPACE_TOOL( D3PD, TrigCostSequenceFillerTool )
	DECLARE_NAMESPACE_TOOL( D3PD, TrigCostTrigElementFillerTool )
}
