#include "src/PixelCalibDbTool.h"
#include "src/PixelDistortionsTool.h"
#ifndef SIMULATIONBASE
#include "src/PixelConditionsSummaryTool.h"
#include "src/PixelDCSConditionsTool.h"
// STSTST #include "src/PixelDeadMapConditionsTool.h"
// STSTST #include "src/PixelTDAQConditionsTool.h"
#endif

DECLARE_COMPONENT( PixelCalibDbTool )
DECLARE_COMPONENT( PixelDistortionsTool )
#ifndef SIMULATIONBASE
DECLARE_COMPONENT( PixelConditionsSummaryTool )
DECLARE_COMPONENT( PixelDCSConditionsTool )
// STSTST DECLARE_COMPONENT( PixelDeadMapConditionsTool )
// STSTST DECLARE_COMPONENT( PixelTDAQConditionsTool )
#endif
