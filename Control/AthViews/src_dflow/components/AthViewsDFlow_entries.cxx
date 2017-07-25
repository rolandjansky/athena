#include "../DFlowAlg1.h"
#include "../DFlowAlg2.h"
#include "../DFlowAlg3.h"
#include "../ViewTestAlg.h"
#include "../ViewMergeAlg.h"
#include "../ViewSubgraphAlg.h"
#include "../RoiCollectionToViews.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, DFlowAlg1 )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, DFlowAlg2 )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, DFlowAlg3 )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, ViewTestAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, ViewMergeAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, ViewSubgraphAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, RoiCollectionToViews )

DECLARE_FACTORY_ENTRIES( AthViewsDFlow )
{
    DECLARE_NAMESPACE_ALGORITHM( AthViews, DFlowAlg1 )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, DFlowAlg2 )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, DFlowAlg3 )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, ViewTestAlg )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, ViewMergeAlg )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, ViewSubgraphAlg )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, RoiCollectionToViews )
}

