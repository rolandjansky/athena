#include "GaudiKernel/DeclareFactoryEntries.h"
#include "iPatTrackFitter/BremFitter.h"
#include "iPatTrackFitter/TrackFitter.h"

DECLARE_TOOL_FACTORY( BremFitter )
DECLARE_TOOL_FACTORY( TrackFitter )
DECLARE_FACTORY_ENTRIES( iPatTrackFitter )
{
    DECLARE_TOOL( BremFitter )
    DECLARE_TOOL( TrackFitter )	
}
