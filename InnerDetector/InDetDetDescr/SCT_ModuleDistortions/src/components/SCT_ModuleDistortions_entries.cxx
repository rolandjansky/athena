#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../SCT_DistortionsTool.h"
#include "../SCT_TestDistortionsTool.h"

DECLARE_TOOL_FACTORY(SCT_DistortionsTool)
DECLARE_ALGORITHM_FACTORY(SCT_TestDistortionsTool)

DECLARE_FACTORY_ENTRIES(SCT_ModuleDistortions) {
  DECLARE_TOOL( SCT_DistortionsTool )
  DECLARE_ALGORITHM( SCT_TestDistortionsTool )
}

