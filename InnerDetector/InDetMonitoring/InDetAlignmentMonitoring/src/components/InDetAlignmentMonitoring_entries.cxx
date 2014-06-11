#include "InDetAlignmentMonitoring/IDAlignMonEfficiencies.h"
#include "InDetAlignmentMonitoring/IDAlignMonGenericTracks.h"
#include "InDetAlignmentMonitoring/InDetAlignMonBeamSpot.h"
#include "InDetAlignmentMonitoring/IDAlignMonResiduals.h"
#include "InDetAlignmentMonitoring/IDAlignMonTruthComparison.h"
#include "InDetAlignmentMonitoring/IDAlignMonNtuple.h" 
#include "InDetAlignmentMonitoring/IDAlignMonTrackSegments.h" 
#include "InDetAlignmentMonitoring/IDAlignMonSivsTRT.h" 
#include "InDetAlignmentMonitoring/TrackSelectionTool.h" 
#include "InDetAlignmentMonitoring/TrackSelectionAlg.h" 

#include "GaudiKernel/DeclareFactoryEntries.h"

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

DECLARE_FACTORY_ENTRIES(InDetAlignmentMonitoring) {
  DECLARE_NAMESPACE_TOOL(InDetAlignMon, TrackSelectionTool);
}
