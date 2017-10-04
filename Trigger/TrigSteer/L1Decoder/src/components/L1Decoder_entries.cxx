#include "../L1CaloDecoder.h"
#include "../FakeRoI.h"
#include "../FakeRoIView.h"
#include "../L1Decoder.h"
#include "../FakeCTP.h"
#include "../CTPUnpackingToolBase.h"
#include "../CTPUnpackingTool.h"
#include "../CTPUnpackingEmulationTool.h"
#include "../EMRoIsUnpackingTool.h"
#include "../JRoIsUnpackingTool.h"
#include "../TAURoIsUnpackingTool.h"
#include "../RoIsUnpackingToolBase.h"
#include "../RoIsUnpackingEmulationTool.h"
#include "../MURoIsUnpackingTool.h"

DECLARE_ALGORITHM_FACTORY(L1CaloDecoder)
DECLARE_ALGORITHM_FACTORY(FakeRoI)
DECLARE_ALGORITHM_FACTORY(FakeCTP)
DECLARE_ALGORITHM_FACTORY(L1Decoder)
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews, FakeRoIView )
DECLARE_TOOL_FACTORY(CTPUnpackingToolBase)
DECLARE_TOOL_FACTORY(CTPUnpackingTool)
DECLARE_TOOL_FACTORY(CTPUnpackingEmulationTool)
DECLARE_TOOL_FACTORY(EMRoIsUnpackingTool)
DECLARE_TOOL_FACTORY(RoIsUnpackingToolBase)
DECLARE_TOOL_FACTORY(RoIsUnpackingEmulationTool)
DECLARE_TOOL_FACTORY(MURoIsUnpackingTool)
DECLARE_TOOL_FACTORY(JRoIsUnpackingTool)
DECLARE_TOOL_FACTORY(TAURoIsUnpackingTool)

