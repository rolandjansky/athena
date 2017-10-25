#include "../BkgStreamsCache.h"
#include "../BkgStreamsConcentricCache.h"
#include "../BkgStreamsStepCache.h"
#include "../SplitBkgStreamsCache.h"
#include "../PileUpEventLoopMgr.h"
#include "../PileUpXingFolder.h"
#include "../PileUpToolsAlg.h"
#include "../DigitizationAlg.h"
#include "../TestPileUpTool.h"
#include "../FlatBM.h"
#include "../ArrayBM.h"
#include "../StepArrayBM.h"
#include "../FixedArrayBM.h"
#include "../LumiProfileSvc.h"
#include "../NoProfileSvc.h"

DECLARE_SERVICE_FACTORY(PileUpEventLoopMgr)
DECLARE_SERVICE_FACTORY(FlatBM)
DECLARE_SERVICE_FACTORY(ArrayBM)
DECLARE_SERVICE_FACTORY(FixedArrayBM)
DECLARE_SERVICE_FACTORY(StepArrayBM)
DECLARE_SERVICE_FACTORY(LumiProfileSvc)
DECLARE_SERVICE_FACTORY(NoProfileSvc)
DECLARE_TOOL_FACTORY(BkgStreamsCache)
DECLARE_TOOL_FACTORY(BkgStreamsConcentricCache)
DECLARE_TOOL_FACTORY(BkgStreamsStepCache)
DECLARE_TOOL_FACTORY(SplitBkgStreamsCache)
DECLARE_TOOL_FACTORY( PileUpXingFolder )
DECLARE_TOOL_FACTORY( TestPileUpTool )
DECLARE_ALGORITHM_FACTORY( PileUpToolsAlg )
DECLARE_ALGORITHM_FACTORY( DigitizationAlg )

