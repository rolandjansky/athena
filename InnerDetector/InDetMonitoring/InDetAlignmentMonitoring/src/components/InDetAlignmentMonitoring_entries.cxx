#include "src/IDAlignMonEfficiencies.h"
#include "src/IDAlignMonGenericTracks.h"
#include "src/InDetAlignMonBeamSpot.h"
#include "src/IDAlignMonResiduals.h"
#include "src/IDAlignMonTruthComparison.h"
#include "src/IDAlignMonNtuple.h" 
#include "src/IDAlignMonTrackSegments.h" 
#include "src/IDAlignMonSivsTRT.h" 
#include "src/TrackSelectionTool.h" 
#include "src/TrackSelectionAlg.h" 
#include "src/IDAlignMonPVBiases.h"

DECLARE_TOOL_FACTORY(IDAlignMonPVBiases)
DECLARE_TOOL_FACTORY(IDAlignMonEfficiencies)
DECLARE_TOOL_FACTORY(IDAlignMonGenericTracks)
DECLARE_TOOL_FACTORY(InDetAlignMonBeamSpot)
DECLARE_TOOL_FACTORY(IDAlignMonResiduals)
DECLARE_TOOL_FACTORY(IDAlignMonTruthComparison)
DECLARE_TOOL_FACTORY(IDAlignMonNtuple)
DECLARE_TOOL_FACTORY(IDAlignMonTrackSegments)
DECLARE_TOOL_FACTORY(IDAlignMonSivsTRT)
DECLARE_NAMESPACE_TOOL_FACTORY(InDetAlignMon, TrackSelectionTool)
DECLARE_ALGORITHM_FACTORY(TrackSelectionAlg)

