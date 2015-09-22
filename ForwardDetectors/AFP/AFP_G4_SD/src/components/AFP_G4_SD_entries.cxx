#include "../AFP_SensitiveDetectorTool.h" //TODO remove once separate versions tested
#include "../AFP_SiDSensitiveDetectorTool.h"
#include "../AFP_TDSensitiveDetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(AFP_SensitiveDetectorTool) //TODO remove once separate versions tested
DECLARE_TOOL_FACTORY(AFP_SiDSensitiveDetectorTool)
DECLARE_TOOL_FACTORY(AFP_TDSensitiveDetectorTool)

DECLARE_FACTORY_ENTRIES(AFP_G4_SD) {
  DECLARE_TOOL(AFP_SensitiveDetectorTool); //TODO remove once separate versions tested
  DECLARE_TOOL(AFP_SiDSensitiveDetectorTool);
  DECLARE_TOOL(AFP_TDSensitiveDetectorTool);
}
