#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuGirlStau/StauTool.h"
#include "MuGirlStau/StauBetaTofTool.h"

using namespace MuGirlNS;

DECLARE_TOOL_FACTORY (StauTool)
DECLARE_TOOL_FACTORY (StauBetaTofTool)

DECLARE_FACTORY_ENTRIES( MuGirlStau )
{
	DECLARE_TOOL( StauTool )
	DECLARE_TOOL( StauBetaTofTool )
}

