#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../HGTDSensorSDTool.h"

DECLARE_TOOL_FACTORY( HGTDSensorSDTool )

DECLARE_FACTORY_ENTRIES( HGTD_G4_SD ) {
  DECLARE_TOOL( HGTDSensorSDTool )
}
