#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../CscCalibMonToolBase.h"
#include "../CscCalibMonToolPed.h"
#include "../CscCalibMonToolSlope.h"

//#include "MuonRawDataMonitoring/MuonEventInfoMonTool.h"
 
DECLARE_TOOL_FACTORY(CscCalibMonToolBase)
DECLARE_TOOL_FACTORY(CscCalibMonToolPed)
DECLARE_TOOL_FACTORY(CscCalibMonToolSlope)
   
DECLARE_FACTORY_ENTRIES(MuonCalibMonitoring)
{
   DECLARE_ALGTOOL(CscCalibMonToolBase)
   DECLARE_ALGTOOL(CscCalibMonToolPed)
   DECLARE_ALGTOOL(CscCalibMonToolSlope)
   
} 
 
