#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../StripAmp.h"
#include "../StripFrontEnd.h"
#include "../StripDigitizationTool.h"
#include "../StripSurfaceChargesGenerator.h"

DECLARE_TOOL_FACTORY(StripAmp)
DECLARE_TOOL_FACTORY(StripFrontEnd)
DECLARE_TOOL_FACTORY(StripDigitizationTool)
DECLARE_TOOL_FACTORY(StripSurfaceChargesGenerator)

DECLARE_FACTORY_ENTRIES(StripDigitization) {
  DECLARE_ALGTOOL(StripAmp)
  DECLARE_ALGTOOL(StripFrontEnd)
  DECLARE_ALGTOOL(StripDigitizationTool)
  DECLARE_ALGTOOL(StripSurfaceChargesGenerator)
}
