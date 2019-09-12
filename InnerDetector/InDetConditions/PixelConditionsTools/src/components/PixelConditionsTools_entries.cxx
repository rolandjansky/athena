#include "src/PixelCalibDbTool.h"
#include "src/PixelDistortionsTool.h"
#include "src/PixelConditionsSummaryTool.h"
#ifndef SIMULATIONBASE
#include "src/PixelByteStreamErrorsTool.h"
#endif

DECLARE_COMPONENT( PixelCalibDbTool )
DECLARE_COMPONENT( PixelDistortionsTool )
DECLARE_COMPONENT( PixelConditionsSummaryTool )
#ifndef SIMULATIONBASE
DECLARE_COMPONENT( PixelByteStreamErrorsTool )
#endif
