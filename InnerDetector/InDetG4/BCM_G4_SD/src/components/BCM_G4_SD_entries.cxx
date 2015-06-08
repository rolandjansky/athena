#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../BCMSensorSDTool.h"

DECLARE_TOOL_FACTORY( BCMSensorSDTool )

DECLARE_FACTORY_ENTRIES( BCM_G4_SD ) {
  DECLARE_TOOL( BCMSensorSDTool )
}
