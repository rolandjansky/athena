#include "../LUCID_SensitiveDetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(LUCID_SensitiveDetectorTool)

DECLARE_FACTORY_ENTRIES(LUCID_G4_SD) {
  DECLARE_TOOL(LUCID_SensitiveDetectorTool);
}
