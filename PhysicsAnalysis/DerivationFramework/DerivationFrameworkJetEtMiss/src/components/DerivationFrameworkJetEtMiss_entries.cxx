#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../JetDecorAlg.h"
#include "../OverlapRemovalECAlg.h"
#include "../JetAugmentationTool.h"
#include "../PFlowAugmentationTool.h"
#include "../BadBatmanAugmentationTool.h"
#include "../METTriggerAugmentationTool.h"
#include "../ViewContainerThinning.h"
#include "../JetExternalAssocTool.h"

using namespace DerivationFramework;
 
DECLARE_ALGORITHM_FACTORY( JetDecorAlg )
DECLARE_ALGORITHM_FACTORY( OverlapRemovalECAlg )
DECLARE_TOOL_FACTORY( JetAugmentationTool )
DECLARE_TOOL_FACTORY( PFlowAugmentationTool )
DECLARE_TOOL_FACTORY( BadBatmanAugmentationTool )
DECLARE_TOOL_FACTORY( METTriggerAugmentationTool )
DECLARE_TOOL_FACTORY( ViewContainerThinning )
DECLARE_TOOL_FACTORY( JetExternalAssocTool )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkJetEtMiss ) {

    DECLARE_ALGORITHM( JetDecorAlg )
    DECLARE_ALGORITHM( OverlapRemovalECAlg )
    DECLARE_TOOL( JetAugmentationTool )
    DECLARE_TOOL( PFlowAugmentationTool )
    DECLARE_TOOL( BadBatmanAugmentationTool )
    DECLARE_TOOL( METTriggerAugmentationTool )
    DECLARE_TOOL( ViewContainerThinning )
    DECLARE_TOOL( JetExternalAssocTool )

}
