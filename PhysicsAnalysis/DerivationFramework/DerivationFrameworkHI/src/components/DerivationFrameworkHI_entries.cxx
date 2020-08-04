#include "DerivationFrameworkHI/HIMBSkimmingTool.h"
#include "DerivationFrameworkHI/HIGapSkimmingTool.h"
#include "DerivationFrameworkHI/HITrackQualityAugmentationTool.h"
#include "DerivationFrameworkHI/HIHijingParsAugmentationTool.h"
#include "DerivationFrameworkHI/HIEventShapeAugmentationTool.h"
#include "DerivationFrameworkHI/HITrackParticleThinningTool.h"
#include "DerivationFrameworkHI/BtaggedObjectThinning.h"
#include "DerivationFrameworkHI/HIGlobalAugmentationTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace DerivationFramework;
DECLARE_TOOL_FACTORY( HIMBSkimmingTool )
DECLARE_TOOL_FACTORY( HIGapSkimmingTool )
DECLARE_TOOL_FACTORY( HITrackQualityAugmentationTool )
DECLARE_TOOL_FACTORY( HIHijingParsAugmentationTool )
DECLARE_TOOL_FACTORY( HIEventShapeAugmentationTool )
DECLARE_TOOL_FACTORY( HITrackParticleThinningTool )
DECLARE_TOOL_FACTORY( HIGlobalAugmentationTool )
DECLARE_TOOL_FACTORY( BtaggedObjectThinning )
DECLARE_FACTORY_ENTRIES( DerivationFrameworkHI )
{
  DECLARE_TOOL( HIMBSkimmingTool )
  DECLARE_TOOL( HIGapSkimmingTool )
  DECLARE_TOOL( HITrackQualityAugmentationTool )
  DECLARE_TOOL( HIHijingParsAugmentationTool )
  DECLARE_TOOL( HIEventShapeAugmentationTool )
  DECLARE_TOOL( HITrackParticleThinningTool )
  DECLARE_TOOL( HIGlobalAugmentationTool )
  DECLARE_TOOL( BtaggedObjectThinning )
}

