#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkExotics/SkimmingToolEXOT15.h"
#include "DerivationFrameworkExotics/SkimmingToolEXOT14.h"
#include "DerivationFrameworkExotics/SkimmingToolEXOT5.h"
#include "DerivationFrameworkExotics/SkimmingToolEXOT20.h"
#include "DerivationFrameworkExotics/BCDistanceAugmentationTool.h"
#include "DerivationFrameworkExotics/MergeMCAna.h"
#include "DerivationFrameworkExotics/SumEvtWeightFilterAlg.h"
#include "DerivationFrameworkExotics/BJetRegressionVariables.h"
#include "DerivationFrameworkExotics/TruthHelper.h"

using namespace DerivationFramework;

DECLARE_COMPONENT( SumEvtWeightFilterAlg )
DECLARE_TOOL_FACTORY( SkimmingToolEXOT15 )
DECLARE_TOOL_FACTORY( SkimmingToolEXOT14 )
DECLARE_TOOL_FACTORY( SkimmingToolEXOT5 )
DECLARE_TOOL_FACTORY( SkimmingToolEXOT20 )
DECLARE_TOOL_FACTORY( BCDistanceAugmentationTool )
DECLARE_TOOL_FACTORY( BJetRegressionVariables )
DECLARE_ALGORITHM_FACTORY( MergeMCAna )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkExotics ) {
   DECLARE_TOOL( SkimmingToolEXOT15 )
   DECLARE_TOOL( SkimmingToolEXOT14 )
   DECLARE_TOOL( SkimmingToolEXOT5 )
   DECLARE_TOOL( SkimmingToolEXOT20 )
   DECLARE_TOOL( BCDistanceAugmentationTool )
   DECLARE_TOOL( BJetRegressionVariables )
   DECLARE_ALGORITHM( MergeMCAna );
}

