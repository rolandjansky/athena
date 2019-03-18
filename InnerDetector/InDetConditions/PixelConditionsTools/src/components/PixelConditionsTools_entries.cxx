#include "src/PixelCalibDbTool.h"
#include "src/PixelDistortionsTool.h"
#ifndef SIMULATIONBASE
#include "src/PixelConditionsSummaryTool.h"
#endif

DECLARE_COMPONENT( PixelCalibDbTool )
DECLARE_COMPONENT( PixelDistortionsTool )
#ifndef SIMULATIONBASE
DECLARE_COMPONENT( PixelConditionsSummaryTool )
#endif
