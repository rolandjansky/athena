#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrackCaloClusterRecTools/ParticleToCaloExtrapolationTool.h"
#include "TrackCaloClusterRecTools/TrackCaloClusterWeightsTool.h"
#include "TrackCaloClusterRecTools/TrackCaloClusterCreatorTool.h"
 
DECLARE_TOOL_FACTORY(ParticleToCaloExtrapolationTool)
DECLARE_TOOL_FACTORY(TrackCaloClusterWeightsTool)
DECLARE_TOOL_FACTORY(TrackCaloClusterCreatorTool)
    
DECLARE_FACTORY_ENTRIES(TrackCaloClusterRecTools)
{
  DECLARE_TOOL(ParticleToCaloExtrapolationTool)      
  DECLARE_TOOL(TrackCaloClusterWeightsTool)   
  DECLARE_TOOL(TrackCaloClusterCreatorTool)   
}