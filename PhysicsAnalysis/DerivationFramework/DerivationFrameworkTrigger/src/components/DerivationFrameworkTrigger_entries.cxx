#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkTrigger/EnhancedBiasAugmentationTool.h"
using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( EnhancedBiasAugmentationTool )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkTrigger ) {
   DECLARE_TOOL( EnhancedBiasAugmentationTool )
}

