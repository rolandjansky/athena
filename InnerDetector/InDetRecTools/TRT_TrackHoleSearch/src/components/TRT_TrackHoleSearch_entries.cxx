#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_TrackHoleSearch/TRTTrackHoleSearchTool.h"
#include "TRT_TrackHoleSearch/TRTStrawEfficiency.h"

DECLARE_TOOL_FACTORY( TRTTrackHoleSearchTool )
DECLARE_ALGORITHM_FACTORY( TRTStrawEfficiency )
  
DECLARE_FACTORY_ENTRIES( TRT_CalibAlgs ) 
{
    DECLARE_TOOL( TRTTrackHoleSearchTool )
    DECLARE_ALGORITHM( TRTStrawEfficiency )
}

