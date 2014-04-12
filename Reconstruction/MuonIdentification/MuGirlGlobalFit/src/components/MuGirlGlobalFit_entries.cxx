#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuGirlGlobalFit/GlobalFitTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( MuGirlNS, GlobalFitTool )

DECLARE_FACTORY_ENTRIES( GlobalFitTool )
{
	DECLARE_NAMESPACE_TOOL( MuGirlNS, GlobalFitTool )
}

