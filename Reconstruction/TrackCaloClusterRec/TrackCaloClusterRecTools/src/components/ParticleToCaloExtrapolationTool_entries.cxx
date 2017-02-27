#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrackCaloClusterRecTools/ParticleToCaloExtrapolationTool.h"
 
DECLARE_TOOL_FACTORY(ParticleToCaloExtrapolationTool)
    
DECLARE_FACTORY_ENTRIES(TrackCaloClusterRecTools)
{
  DECLARE_TOOL(ParticleToCaloExtrapolationTool)      
}