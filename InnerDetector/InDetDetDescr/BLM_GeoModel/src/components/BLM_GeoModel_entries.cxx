#include "GaudiKernel/DeclareFactoryEntries.h"
#include "BLM_GeoModel/BLM_Builder.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDetDD, BLM_Builder )

DECLARE_FACTORY_ENTRIES( BLM_GeoModel )
{
	DECLARE_NAMESPACE_TOOL( InDetDD, BLM_Builder )
}
