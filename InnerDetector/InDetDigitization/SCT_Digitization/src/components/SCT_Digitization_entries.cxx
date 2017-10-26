#include "../SCT_Amp.h"
#include "../SCT_FrontEnd.h"
#include "SCT_Digitization/SCT_Digitization.h"
#include "SCT_Digitization/SCT_DigitizationTool.h"
#include "../SCT_SurfaceChargesGenerator.h"
#include "../SCT_DetailedSurfaceChargesGenerator.h"
#include "../SCT_RandomDisabledCellGenerator.h"

DECLARE_ALGORITHM_FACTORY(SCT_Digitization)
DECLARE_TOOL_FACTORY(SCT_Amp)  
DECLARE_TOOL_FACTORY(SCT_FrontEnd)
DECLARE_TOOL_FACTORY(SCT_DigitizationTool)
DECLARE_TOOL_FACTORY(SCT_SurfaceChargesGenerator)
DECLARE_TOOL_FACTORY(SCT_DetailedSurfaceChargesGenerator)
DECLARE_TOOL_FACTORY(SCT_RandomDisabledCellGenerator)

