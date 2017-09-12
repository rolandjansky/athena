
#include "GaudiKernel/DeclareFactoryEntries.h"


#include "../TestFEXAlg.h"
#include "../TestFEXAlgView.h"
#include "../TestMergeAlg.h"
#include "../TestSplitAlg.h"
#include "../MergeRoIsAlg.h"
#include "../TestViewDriver.h"
#include "../TestViewMerger.h"
#include "../SchedulerProxyAlg.h"
#include "../TestCombiner.h"


DECLARE_ALGORITHM_FACTORY( TestFEXAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, TestFEXAlgView )
DECLARE_ALGORITHM_FACTORY( TestMergeAlg )
DECLARE_ALGORITHM_FACTORY( TestSplitAlg )
DECLARE_ALGORITHM_FACTORY( MergeRoIsAlg )
DECLARE_ALGORITHM_FACTORY( TestViewDriver )
DECLARE_ALGORITHM_FACTORY( TestViewMerger )
DECLARE_ALGORITHM_FACTORY( SchedulerProxyAlg )
DECLARE_ALGORITHM_FACTORY( TestCombiner )

DECLARE_FACTORY_ENTRIES( ViewAlgsTest )
{
  DECLARE_ALGORITHM( TestFEXAlg )
  DECLARE_NAMESPACE_ALGORITHM( AthViews, TestFEXAlgView )
  DECLARE_ALGORITHM( TestMergeAlg )
  DECLARE_ALGORITHM( TestSplitAlg )
  DECLARE_ALGORITHM( MergeRoIsAlg )
  DECLARE_ALGORITHM( TestViewDriver )
  DECLARE_ALGORITHM( TestViewMerger )
  DECLARE_ALGORITHM( SchedulerProxyAlg )
  DECLARE_ALGORITHM( TestCombiner )
}
