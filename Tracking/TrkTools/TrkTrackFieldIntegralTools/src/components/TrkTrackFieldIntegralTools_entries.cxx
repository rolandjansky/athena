#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../FieldIntegralByTrackQueryTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, FieldIntegralByTrackQueryTool )

DECLARE_FACTORY_ENTRIES( TrkTrackFieldIntegralTools )
{
	DECLARE_NAMESPACE_TOOL( Trk, FieldIntegralByTrackQueryTool )
}

