#include "GaudiKernel/DeclareFactoryEntries.h"

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


DECLARE_FACTORY_ENTRIES(PileUpComps) {
    DECLARE_SERVICE( PileUpEventLoopMgr )
    DECLARE_SERVICE( FlatBM )
    DECLARE_SERVICE( ArrayBM )
    DECLARE_SERVICE( FixedArrayBM )
    DECLARE_SERVICE( StepArrayBM )
    DECLARE_SERVICE( LumiProfileSvc )
    DECLARE_SERVICE( NoProfileSvc )
    DECLARE_ALGTOOL( BkgStreamsCache )
    DECLARE_ALGTOOL( BkgStreamsConcentricCache )
    DECLARE_ALGTOOL( BkgStreamsStepCache )
    DECLARE_ALGTOOL( SplitBkgStreamsCache )
    DECLARE_ALGTOOL( PileUpXingFolder )
    DECLARE_ALGTOOL( TestPileUpTool )
    DECLARE_ALGORITHM( PileUpToolsAlg )
    DECLARE_ALGORITHM( DigitizationAlg )
}
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
