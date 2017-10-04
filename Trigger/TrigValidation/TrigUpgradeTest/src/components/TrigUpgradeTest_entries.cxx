#include "../ITestHypoTool.h"
#include "../TestHypoAlg.h"
#include "../TestHypoTool.h"
#include "../TestMHypoTool.h"
#include "../TestRecoAlg.h"
#include "../TestRoRSeqFilter.h"
#include "../TestMerger.h"
#include "../TestComboHypoAlg.h"
#include "../TestInputMaker.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY ( HLTTest, TestInputMaker )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( HLTTest, TestHypoAlg )
DECLARE_NAMESPACE_TOOL_FACTORY      ( HLTTest, TestHypoTool )
DECLARE_NAMESPACE_TOOL_FACTORY      ( HLTTest, TestMHypoTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( HLTTest, TestRecoAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( HLTTest, TestRoRSeqFilter )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( HLTTest, TestMerger )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( HLTTest, TestComboHypoAlg )
