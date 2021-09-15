#include "GaudiKernel/DeclareFactoryEntries.h"

#include "HGTD_Digitization/HGTD_DigitizationTool.h"
#include "HGTD_Digitization/HGTD_FrontEndTool.h"
#include "HGTD_Digitization/HGTD_SurfaceChargesGenerator.h"

DECLARE_TOOL_FACTORY(HGTD_DigitizationTool)
DECLARE_TOOL_FACTORY(HGTD_SurfaceChargesGenerator)
DECLARE_TOOL_FACTORY(HGTD_FrontEndTool)

DECLARE_FACTORY_ENTRIES(HGTD_Digitization) {
  DECLARE_ALGTOOL(HGTD_DigitizationTool)
  DECLARE_ALGTOOL(HGTD_SurfaceChargesGenerator)
  DECLARE_ALGTOOL(HGTD_FrontEndTool)
}
