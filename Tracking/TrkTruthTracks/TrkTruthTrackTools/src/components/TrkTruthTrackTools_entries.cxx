#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TruthTrackBuilder.h"
#include "../PRD_TruthTrajectoryBuilder.h"

 
DECLARE_NAMESPACE_TOOL_FACTORY(Trk,TruthTrackBuilder)
DECLARE_NAMESPACE_TOOL_FACTORY(Trk,PRD_TruthTrajectoryBuilder)
    
DECLARE_FACTORY_ENTRIES(TrkTruthTrackTools)
{
    DECLARE_NAMESPACE_TOOL(Trk,TruthTrackBuilder)      
    DECLARE_NAMESPACE_TOOL(Trk,PRD_TruthTrajectoryBuilder)      
}
