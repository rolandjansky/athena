#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../JetDecorAlg.h"
#include "../JetAugmentationTool.h"
#include "../PFlowAugmentationTool.h"
#include "../BadBatmanAugmentationTool.h"
#include "../DistanceInTrainAugmentationTool.h"
#include "../METTriggerAugmentationTool.h"
#include "../ViewContainerThinning.h"
#include "../JetExternalAssocTool.h"
#include "../TVAAugmentationTool.h"

using namespace DerivationFramework;
 
DECLARE_ALGORITHM_FACTORY( JetDecorAlg )
DECLARE_TOOL_FACTORY( JetAugmentationTool )
DECLARE_TOOL_FACTORY( PFlowAugmentationTool )
DECLARE_TOOL_FACTORY( BadBatmanAugmentationTool )
DECLARE_TOOL_FACTORY( DistanceInTrainAugmentationTool )
DECLARE_TOOL_FACTORY( METTriggerAugmentationTool )
DECLARE_TOOL_FACTORY( ViewContainerThinning )
DECLARE_TOOL_FACTORY( JetExternalAssocTool )
DECLARE_TOOL_FACTORY( TVAAugmentationTool )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkJetEtMiss ) {

    DECLARE_ALGORITHM( JetDecorAlg )
    DECLARE_TOOL( JetAugmentationTool )
    DECLARE_TOOL( PFlowAugmentationTool )
    DECLARE_TOOL( BadBatmanAugmentationTool )
    DECLARE_TOOL( DistanceInTrainAugmentationTool )
    DECLARE_TOOL( METTriggerAugmentationTool )
    DECLARE_TOOL( ViewContainerThinning )
    DECLARE_TOOL( JetExternalAssocTool )
    DECLARE_TOOL( TVAAugmentationTool )

}
