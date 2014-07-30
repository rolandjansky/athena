#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkTrackSlimmingTool/TrackSlimmingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TrackSlimmingTool )

DECLARE_FACTORY_ENTRIES( TrkTrackSlimmingTool )
{
	DECLARE_NAMESPACE_TOOL( Trk, TrackSlimmingTool )
}

