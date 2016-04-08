#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonCombinedTimingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuonCombinedTimingTool )

DECLARE_FACTORY_ENTRIES( MuonCombinedTimingTools )
{
	DECLARE_NAMESPACE_TOOL( Rec, MuonCombinedTimingTool );
}

