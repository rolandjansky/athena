#include "GaudiKernel/DeclareFactoryEntries.h"
#include "BCM_GeoModel/BCM_Builder.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDetDD, BCM_Builder )

DECLARE_FACTORY_ENTRIES( BCM_GeoModel )
{
	DECLARE_NAMESPACE_TOOL( InDetDD, BCM_Builder )
}
