#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../BarrelFastSimDedicatedSD.h"
// #include "../DeadMaterialShower.h"
#include "../EndcapFastSimDedicatedSD.h"
#include "../FCALFastSimDedicatedSD.h"
#include "../LArFastShowerTool.h"

DECLARE_TOOL_FACTORY( BarrelFastSimDedicatedSD )
// DECLARE_TOOL_FACTORY( DeadMaterialShower )
DECLARE_TOOL_FACTORY( EndcapFastSimDedicatedSD )
DECLARE_TOOL_FACTORY( FCALFastSimDedicatedSD )
DECLARE_TOOL_FACTORY( LArFastShowerTool )

DECLARE_FACTORY_ENTRIES( LArG4FastSimulation ) {
  DECLARE_TOOL( BarrelFastSimDedicatedSD )
  // DECLARE_TOOL( DeadMaterialShower )
  DECLARE_TOOL( EndcapFastSimDedicatedSD )
  DECLARE_TOOL( FCALFastSimDedicatedSD )
  DECLARE_TOOL( LArFastShowerTool )
}
