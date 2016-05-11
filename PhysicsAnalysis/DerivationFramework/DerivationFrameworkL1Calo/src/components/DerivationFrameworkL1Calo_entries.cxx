#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkL1Calo/SaturatedTriggerTower.h"
#include "xAODTrigL1Calo/L1TopoRawDataContainer.h"
#include "xAODTrigL1Calo/L1TopoRawDataAuxContainer.h"


DECLARE_NAMESPACE_TOOL_FACTORY( DerivationFramework , SaturatedTriggerTower )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkL1Calo ) {
   DECLARE_NAMESPACE_TOOL( DerivationFramework , SaturatedTriggerTower )
}
 