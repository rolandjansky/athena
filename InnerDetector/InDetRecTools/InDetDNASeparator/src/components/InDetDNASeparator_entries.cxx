#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetDNASeparator/InDetDNASeparator.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetDNASeparator )

DECLARE_FACTORY_ENTRIES( InDetDNASeparator )
{
	DECLARE_NAMESPACE_TOOL( InDet, InDetDNASeparator )
}

