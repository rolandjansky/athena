#include "InDetCosmicsEventPhase/InDetCosmicsEventPhase.h"
#include "InDetCosmicsEventPhase/InDetCosmicsEventPhaseTool.h"
#include "InDetCosmicsEventPhase/InDetFixedWindowTrackTimeTool.h"
#include "InDetCosmicsEventPhase/InDetSlidingWindowTrackTimeTool.h"
#include "InDetCosmicsEventPhase/TRT_TrackTimingTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetCosmicsEventPhase )

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetCosmicsEventPhaseTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetFixedWindowTrackTimeTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetSlidingWindowTrackTimeTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRT_TrackTimingTool )

DECLARE_FACTORY_ENTRIES(InDetCosmicsEventPhase) {
  DECLARE_NAMESPACE_ALGORITHM( InDet, InDetCosmicsEventPhase);
    
  DECLARE_NAMESPACE_TOOL( InDet, InDetCosmicsEventPhaseTool );
  DECLARE_NAMESPACE_TOOL( InDet, InDetFixedWidnowTrackTimeTool );
  DECLARE_NAMESPACE_TOOL( InDet, InDetSlidingWidnowTrackTimeTool );
  DECLARE_NAMESPACE_TOOL( InDet, TRT_TrackTimingTool );

}


