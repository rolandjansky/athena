#include "GaudiKernel/DeclareFactoryEntries.h"

// #include "../DeadMaterialShower.h"
#include "../LArFastShowerTool.h"

// DECLARE_TOOL_FACTORY( DeadMaterialShower )
DECLARE_TOOL_FACTORY( LArFastShowerTool )

DECLARE_FACTORY_ENTRIES( LArG4FastSimulation ) {
  // DECLARE_TOOL( DeadMaterialShower )
  DECLARE_TOOL( LArFastShowerTool )
}
