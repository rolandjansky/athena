#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrackCaloClusterRecTools/ParticleToCaloExtrapolationTool.h"
// #include "TrackCaloClusterRecTools/TrackCaloClusterWeightsTool.h"
// #include "TrackCaloClusterRecTools/TrackCaloClusterCreatorTool.h"
#include "TrackCaloClusterRecTools/ClusterFilterTool.h"
#include "TrackCaloClusterRecTools/TrackCaloClusterTool.h"
 
DECLARE_TOOL_FACTORY(ParticleToCaloExtrapolationTool)
// DECLARE_TOOL_FACTORY(TrackCaloClusterWeightsTool)
// DECLARE_TOOL_FACTORY(TrackCaloClusterCreatorTool)
DECLARE_TOOL_FACTORY(ClusterFilterTool)

DECLARE_TOOL_FACTORY(TCCCombinedTool)
DECLARE_TOOL_FACTORY(TCCChargedTool)
DECLARE_TOOL_FACTORY(TCCNeutralTool)
DECLARE_TOOL_FACTORY(UFOTool)

DECLARE_FACTORY_ENTRIES(TrackCaloClusterRecTools)
{
  DECLARE_TOOL(ParticleToCaloExtrapolationTool)      
  // DECLARE_TOOL(TrackCaloClusterWeightsTool)   
  // DECLARE_TOOL(TrackCaloClusterCreatorTool)  
  DECLARE_TOOL(ClusterFilterTool)  

  DECLARE_TOOL(TCCCombinedTool)
  DECLARE_TOOL(TCCChargedTool)
  DECLARE_TOOL(TCCNeutralTool)
  DECLARE_TOOL(UFOTool)
  }
