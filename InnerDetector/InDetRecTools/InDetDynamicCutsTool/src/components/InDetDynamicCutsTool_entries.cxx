#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetDynamicCutsTool/InDetDynamicCutsTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetDynamicCutsTool )

DECLARE_FACTORY_ENTRIES( InDetDynamicCutsTool )
{
	DECLARE_NAMESPACE_TOOL( InDet, InDetDynamicCutsTool )
}

