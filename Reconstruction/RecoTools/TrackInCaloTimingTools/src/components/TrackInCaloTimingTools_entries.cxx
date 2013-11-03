#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrackInCaloTimingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rec, TrackInCaloTimingTool )

DECLARE_FACTORY_ENTRIES( TrackInCaloTimingTool ) {
  DECLARE_NAMESPACE_TOOL( Rec, TrackInCaloTimingTool )
}
