#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuGirlSelection/ANNSelectionTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( MuGirlNS, ANNSelectionTool )

DECLARE_FACTORY_ENTRIES( MuGirlSelection )
{
	DECLARE_NAMESPACE_TOOL( MuGirlNS, ANNSelectionTool )
}

