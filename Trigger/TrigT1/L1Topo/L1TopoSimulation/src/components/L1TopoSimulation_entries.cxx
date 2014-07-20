#include "GaudiKernel/DeclareFactoryEntries.h"

#include "L1TopoSimulation/L1TopoSimulation.h"
#include "../EMTauInputProvider.h"
#include "../JetInputProvider.h"
#include "../EnergyInputProvider.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1, L1TopoSimulation )

DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, EMTauInputProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, JetInputProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, EnergyInputProvider )

DECLARE_FACTORY_ENTRIES( L1TopoSimulation ) {

   DECLARE_NAMESPACE_ALGORITHM( LVL1, L1TopoSimulation )

   DECLARE_NAMESPACE_TOOL( LVL1, EMTauInputProvider )
   DECLARE_NAMESPACE_TOOL( LVL1, JetInputProvider )
   DECLARE_NAMESPACE_TOOL( LVL1, EnergyInputProvider )

}
