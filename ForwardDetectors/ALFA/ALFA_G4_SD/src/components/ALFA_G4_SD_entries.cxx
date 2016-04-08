#include "../ALFA_SensitiveDetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(ALFA_SensitiveDetectorTool)

DECLARE_FACTORY_ENTRIES(ALFA_G4_SD) {
  DECLARE_TOOL(ALFA_SensitiveDetectorTool);
}

