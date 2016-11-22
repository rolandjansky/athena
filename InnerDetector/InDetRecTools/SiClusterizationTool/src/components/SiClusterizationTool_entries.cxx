#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
//#include "SCT_ConditionsServices/SCT_ConditionsSummarySvc.h"
//#include "PixelConditionsServices/PixelConditionsSummarySvc.h"

#include "SiClusterizationTool/MergedPixelsTool.h"
#include "SiClusterizationTool/SCT_ClusteringTool.h"
#include "SiClusterizationTool/SCT_ReClustering.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "SiClusterizationTool/TotPixelClusterSplitter.h"
#include "SiClusterizationTool/NnPixelClusterSplitter.h"
#include "SiClusterizationTool/NnClusterizationFactory.h"
#include "SiClusterizationTool/NnPixelClusterSplitProbTool.h"
#include "SiClusterizationTool/TruthPixelClusterSplitter.h"
#include "SiClusterizationTool/TruthClusterizationFactory.h"
#include "SiClusterizationTool/TruthPixelClusterSplitProbTool.h"

using namespace InDet;

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, MergedPixelsTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, SCT_ClusteringTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, PixelGangedAmbiguitiesFinder )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, ClusterMakerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TotPixelClusterSplitter )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, NnPixelClusterSplitter )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, NnClusterizationFactory )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, NnPixelClusterSplitProbTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TruthPixelClusterSplitter )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TruthClusterizationFactory )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TruthPixelClusterSplitProbTool )

DECLARE_FACTORY_ENTRIES( SiClusterizationTool ){

  DECLARE_NAMESPACE_TOOL( InDet, MergedPixelsTool )
  DECLARE_NAMESPACE_TOOL( InDet, SCT_ClusteringTool )
  DECLARE_NAMESPACE_TOOL( InDet, PixelGangedAmbiguitiesFinder )
  DECLARE_NAMESPACE_TOOL( InDet, ClusterMakerTool )
  DECLARE_NAMESPACE_TOOL( InDet, TotPixelClusterSplitter )
  DECLARE_NAMESPACE_TOOL( InDet, NnPixelClusterSplitter )
  DECLARE_NAMESPACE_TOOL( InDet, NnClusterizationFactory )
  DECLARE_NAMESPACE_TOOL( InDet, NnPixelClusterSplitProbTool )
  DECLARE_NAMESPACE_TOOL( InDet, TruthPixelClusterSplitter )
  DECLARE_NAMESPACE_TOOL( InDet, TruthClusterizationFactory )
  DECLARE_NAMESPACE_TOOL( InDet, TruthPixelClusterSplitProbTool )
  
}










