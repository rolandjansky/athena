
#include "GaudiKernel/DeclareFactoryEntries.h"


#include "../TestFEXAlg.h"
#include "../TestFEXAlgView.h"
#include "../TestHypoAlg.h"
#include "../TrackHypoAlg.h"
#include "../TestHypoAlgView.h"
#include "../TestMergeAlg.h"
#include "../TestSplitAlg.h"
#include "../TestHypoTool.h"
#include "../TrackHypoTool.h"
#include "../TrackRoIAssocTool.h"
#include "../MergeRoIsAlg.h"
#include "../FilterTracksInRoIs.h"
#include "../TestViewDriver.h"
#include "../SchedulerProxyAlg.h"




DECLARE_ALGORITHM_FACTORY( TestFEXAlg )
DECLARE_ALGORITHM_FACTORY( FilterTracksInRoIs )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, TestFEXAlgView )
DECLARE_ALGORITHM_FACTORY( TestHypoAlg )
DECLARE_ALGORITHM_FACTORY( TrackHypoAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, TestHypoAlgView )
DECLARE_TOOL_FACTORY( TestHypoTool )
DECLARE_TOOL_FACTORY( TrackHypoTool )
DECLARE_TOOL_FACTORY( TrackRoIAssocTool )
DECLARE_ALGORITHM_FACTORY( TestMergeAlg )
DECLARE_ALGORITHM_FACTORY( TestSplitAlg )
DECLARE_ALGORITHM_FACTORY( MergeRoIsAlg )
DECLARE_ALGORITHM_FACTORY( TestViewDriver )
DECLARE_ALGORITHM_FACTORY( SchedulerProxyAlg )

DECLARE_FACTORY_ENTRIES( ViewAlgsTest )
{
  DECLARE_ALGORITHM( TestFEXAlg )
  DECLARE_ALGORITHM( FilterTracksInRoIs )
  DECLARE_NAMESPACE_ALGORITHM( AthViews, TestFEXAlgView )
  DECLARE_ALGORITHM( TestHypoAlg )
  DECLARE_ALGORITHM( TrackHypoAlg )
  DECLARE_NAMESPACE_ALGORITHM( AthViews, TestHypoAlgView )
  DECLARE_ALGORITHM( TestMergeAlg )
  DECLARE_ALGORITHM( TestSplitAlg )
  DECLARE_ALGORITHM( MergeRoIsAlg )
  DECLARE_ALGORITHM( TestViewDriver )
  DECLARE_ALGORITHM( SchedulerProxyAlg )
  DECLARE_TOOL( TestHypoTool )
  DECLARE_TOOL( TrackHypoTool )
  DECLARE_TOOL( TrackRoIAssocTool )
}
