#include "InDetCosmicsEventPhase/InDetCosmicsEventPhase.h"
#include "InDetCosmicsEventPhase/InDetCosmicsEventPhaseTool.h"
#include "InDetCosmicsEventPhase/InDetFixedWindowTrackTimeTool.h"
#include "InDetCosmicsEventPhase/InDetSlidingWindowTrackTimeTool.h"
#include "InDetCosmicsEventPhase/TRT_TrackTimingTool.h"

using namespace InDet;
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetCosmicsEventPhase )

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetCosmicsEventPhaseTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetFixedWindowTrackTimeTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetSlidingWindowTrackTimeTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRT_TrackTimingTool )

    

