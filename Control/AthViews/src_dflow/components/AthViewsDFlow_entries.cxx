#include "../DFlowAlg1.h"
#include "../DFlowAlg2.h"
#include "../DFlowAlg3.h"
#include "../DFlowAlg1_manualViews.h"
#include "../DFlowAlg2_manualViews.h"
#include "../DFlowAlg3_manualViews.h"
#include "../ViewMergeAlg.h"
#include "../ViewMakeAlg.h"
#include "../ViewSubgraphAlg.h"
//#include "../ViewIncidentMakeAlg.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, DFlowAlg1 )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, DFlowAlg2 )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, DFlowAlg3 )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, DFlowAlg1_manualViews )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, DFlowAlg2_manualViews )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, DFlowAlg3_manualViews )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, ViewMergeAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, ViewMakeAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, ViewSubgraphAlg )
//DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, ViewIncidentMakeAlg )

DECLARE_FACTORY_ENTRIES( AthViewsDFlow )
{
    DECLARE_NAMESPACE_ALGORITHM( AthViews, DFlowAlg1 )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, DFlowAlg2 )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, DFlowAlg3 )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, DFlowAlg1_manualViews )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, DFlowAlg2_manualViews )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, DFlowAlg3_manualViews )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, ViewMergeAlg )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, ViewMakeAlg )
    DECLARE_NAMESPACE_ALGORITHM( AthViews, ViewSubgraphAlg )
    //DECLARE_NAMESPACE_ALGORITHM( AthViews, ViewIncidentMakeAlg )
}

