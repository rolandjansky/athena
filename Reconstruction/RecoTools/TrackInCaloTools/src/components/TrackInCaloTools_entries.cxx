#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrackInCaloTools/TrackInCaloTools.h"
#include "TrackInCaloTools/TrackExtrapolatorToCalo.h"
#include "TrackInCaloTools/TrackDirectionTool.h"

DECLARE_TOOL_FACTORY( TrackInCaloTools )
DECLARE_TOOL_FACTORY( TrackExtrapolatorToCalo )
DECLARE_TOOL_FACTORY( TrackDirectionTool )

DECLARE_FACTORY_ENTRIES( TrackInCaloTools) {
  DECLARE_TOOL( TrackInCaloTools )
}

DECLARE_FACTORY_ENTRIES( TrackExtrapolatorToCalo ) {
  DECLARE_TOOL( TrackExtrapolatorToCalo )
}

DECLARE_FACTORY_ENTRIES( TrackDirectionTool) {
  DECLARE_TOOL( TrackDirectionTool )
}
