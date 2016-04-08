#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../BLMSensorSDTool.h"

DECLARE_TOOL_FACTORY( BLMSensorSDTool )

DECLARE_FACTORY_ENTRIES( BLM_G4_SD ) {
  DECLARE_TOOL( BLMSensorSDTool )
}
