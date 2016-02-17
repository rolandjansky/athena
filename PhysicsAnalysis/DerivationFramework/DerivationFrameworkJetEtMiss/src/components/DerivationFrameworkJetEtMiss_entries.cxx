#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../JetDecorAlg.h"
#include "../JetAugmentationTool.h"
#include "../METTriggerAugmentationTool.h"

using namespace DerivationFramework;
 
DECLARE_ALGORITHM_FACTORY( JetDecorAlg )
DECLARE_TOOL_FACTORY( JetAugmentationTool )
DECLARE_TOOL_FACTORY( METTriggerAugmentationTool )
 
DECLARE_FACTORY_ENTRIES( DerivationFrameworkJetEtMiss ) {

    DECLARE_ALGORITHM( JetDecorAlg )
    DECLARE_TOOL( JetAugmentationTool )
    DECLARE_TOOL( METTriggerAugmentationTool )

}
