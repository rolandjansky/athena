#include "TRT_DetectorPerformance/TRT_DetectorPlots.h"
#include "TRT_DetectorPerformance/TRT_DetectorTiming.h"
#include "TRT_DetectorPerformance/TRT_ClockNoise.h"
#include "TRT_DetectorPerformance/TRT_StrawMap.h"
#include "TRT_DetectorPerformance/TRT_EventPhaseAnalysis.h"
#include "TRT_DetectorPerformance/TRT_DTMROC_HeaderCheck.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_DetectorPlots )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_DetectorTiming )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_ClockNoise )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_EventPhaseAnalysis )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_DTMROC_HeaderCheck )

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRT_StrawMap )

DECLARE_FACTORY_ENTRIES( TRT_DetectorPerformance ) {
  DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_DetectorPlots );
  DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_DetectorTiming );
  DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_ClockNoise );
  DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_EventPhaseAnalysis );
  DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_DTMROC_HeaderCheck );

  DECLARE_NAMESPACE_TOOL( InDet, TRT_StrawMap );
}
