#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkCalo/CaloClusterThinning.h"
#include "DerivationFrameworkCalo/JetCaloClusterThinning.h"
#include "DerivationFrameworkCalo/CellsInConeThinning.h"
#include "DerivationFrameworkCalo/GainDecorator.h"
#include "DerivationFrameworkCalo/MaxCellDecorator.h"
#include "DerivationFrameworkCalo/ClusterEnergyPerLayerDecorator.h"

using namespace DerivationFramework;
 
DECLARE_TOOL_FACTORY( CaloClusterThinning )
DECLARE_TOOL_FACTORY( JetCaloClusterThinning )
DECLARE_TOOL_FACTORY( CellsInConeThinning )
DECLARE_TOOL_FACTORY( GainDecorator )
DECLARE_TOOL_FACTORY( MaxCellDecorator )
DECLARE_TOOL_FACTORY( ClusterEnergyPerLayerDecorator )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkCalo ) {
   DECLARE_TOOL( CaloClusterThinning )
   DECLARE_TOOL( JetCaloClusterThinning )
   DECLARE_TOOL( CellsInConeThinning )
   DECLARE_TOOL( GainDecorator )
   DECLARE_TOOL( MaxCellDecorator )
   DECLARE_TOOL( ClusterEnergyPerLayerDecorator )   
}
 
