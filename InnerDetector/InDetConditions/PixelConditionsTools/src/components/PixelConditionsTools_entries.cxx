#include "src/PixelRecoDbTool.h"
#include "src/PixelCalibDbTool.h"
#include "src/PixelDistortionsTool.h"
#ifndef SIMULATIONBASE
#include "src/PixelConditionsSummaryTool.h"
#include "src/PixelDCSConditionsTool.h"
#endif

DECLARE_COMPONENT( PixelRecoDbTool )
DECLARE_COMPONENT( PixelCalibDbTool )
DECLARE_COMPONENT( PixelDistortionsTool )
#ifndef SIMULATIONBASE
DECLARE_COMPONENT( PixelConditionsSummaryTool )
DECLARE_COMPONENT( PixelDCSConditionsTool )
#endif
