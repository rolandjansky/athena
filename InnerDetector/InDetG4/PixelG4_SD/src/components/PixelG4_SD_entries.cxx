#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../PixelSensorSDTool.h"

DECLARE_TOOL_FACTORY( PixelSensorSDTool )

DECLARE_FACTORY_ENTRIES( PixelG4_SD ) {
  DECLARE_TOOL( PixelSensorSDTool )
}
