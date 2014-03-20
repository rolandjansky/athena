#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuGirlPerformanceTools/PerformanceTruthTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( MuGirlNS, PerformanceTruthTool )

DECLARE_FACTORY_ENTRIES( PerformanceTruthTool )
{
	DECLARE_NAMESPACE_TOOL( MuGirlNS, PerformanceTruthTool )
}

