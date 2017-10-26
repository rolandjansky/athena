#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
//#include "SCT_ConditionsServices/SCT_ConditionsSummarySvc.h"
//#include "PixelConditionsServices/PixelConditionsSummarySvc.h"

#include "SiClusterizationTool/MergedPixelsTool.h"
#include "SiClusterizationTool/SCT_ClusteringTool.h"
#include "SiClusterizationTool/SCT_ReClustering.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"
#include "SiClusterizationTool/TotPixelClusterSplitter.h"
#include "SiClusterizationTool/NnPixelClusterSplitter.h"
#include "SiClusterizationTool/NnClusterizationFactory.h"
#include "SiClusterizationTool/NnPixelClusterSplitProbTool.h"
#include "SiClusterizationTool/TruthPixelClusterSplitter.h"
#include "SiClusterizationTool/TruthClusterizationFactory.h"
#include "SiClusterizationTool/TruthPixelClusterSplitProbTool.h"

using namespace InDet;

DECLARE_COMPONENT( InDet::MergedPixelsTool )
DECLARE_COMPONENT( InDet::SCT_ClusteringTool )
DECLARE_COMPONENT( InDet::PixelGangedAmbiguitiesFinder )
DECLARE_COMPONENT( InDet::ClusterMakerTool )
DECLARE_COMPONENT( InDet::TotPixelClusterSplitter )
DECLARE_COMPONENT( InDet::NnPixelClusterSplitter )
DECLARE_COMPONENT( InDet::NnClusterizationFactory )
DECLARE_COMPONENT( InDet::NnPixelClusterSplitProbTool )
DECLARE_COMPONENT( InDet::TruthPixelClusterSplitter )
DECLARE_COMPONENT( InDet::TruthClusterizationFactory )
DECLARE_COMPONENT( InDet::TruthPixelClusterSplitProbTool )


  

