#include "../TRTCalibTrackSelectionTool.h"
#include "../FillAlignTrkInfo.h"
#include "../FillAlignTRTHits.h"
#include "../TRTCalibrator.h"
//#include "../TRTCalAccumulator.h"
#include "../FitTool.h"
//#include "../TRTCalChipAccumulator.h"
//#include "../TRTCalModuleAccumulator.h"
//#include "../TRTCalStrawAccumulator.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( TRTCalibTrackSelectionTool )
DECLARE_TOOL_FACTORY( FillAlignTrkInfo )
DECLARE_TOOL_FACTORY( FillAlignTRTHits )
DECLARE_TOOL_FACTORY( TRTCalibrator )
//DECLARE_TOOL_FACTORY( TRTCalAccumulator )
DECLARE_TOOL_FACTORY( FitTool )
//DECLARE_TOOL_FACTORY( TRTCalChipAccumulator  )
//DECLARE_TOOL_FACTORY( TRTCalModuleAccumulator  )
//DECLARE_TOOL_FACTORY( TRTCalStrawAccumulator )


DECLARE_FACTORY_ENTRIES( TRT_CalibTools ) 
{
  DECLARE_TOOL( TRTCalibTrackSelectionTool )
  DECLARE_TOOL( FillAlignTrkInfo )
  DECLARE_TOOL( FillAlignTRTHits )
  DECLARE_TOOL( TRTCalibrator )
//  DECLARE_TOOL( TRTCalAccumulator )
  DECLARE_TOOL( FitTool )
//  DECLARE_TOOL( TRTCalChipAccumulator  )
//  DECLARE_TOOL( TRTCalModuleAccumulator  )
//  DECLARE_TOOL( TRTCalStrawAccumulator )
}
