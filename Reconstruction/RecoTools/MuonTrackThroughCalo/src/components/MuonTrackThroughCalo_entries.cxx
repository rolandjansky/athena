#include "../MuonTrackThroughCalo.h"
#include "../TrackThroughDetectorBoundaries.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace Rec;

DECLARE_TOOL_FACTORY( MuonTrackThroughCalo )
DECLARE_TOOL_FACTORY( TrackThroughDetectorBoundaries )

DECLARE_FACTORY_ENTRIES( MuonTrackThroughCalo ){
      DECLARE_TOOL( MuonTrackThroughCalo )
      DECLARE_TOOL( TrackThroughDetectorBoundaries )
}

