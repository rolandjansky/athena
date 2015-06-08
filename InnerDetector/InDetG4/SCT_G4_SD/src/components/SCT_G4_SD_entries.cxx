#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../SctSensorSDTool.h"
#include "../SctSensor_CTBTool.h"

DECLARE_TOOL_FACTORY( SctSensorSDTool )
DECLARE_TOOL_FACTORY( SctSensor_CTBTool )

DECLARE_FACTORY_ENTRIES( SCT_G4_SD ) {
  DECLARE_TOOL( SctSensorSDTool )
  DECLARE_TOOL( SctSensor_CTBTool )
}
