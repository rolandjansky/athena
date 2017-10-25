#include "EventBookkeeperTools/myCppFilterTest.h"
#include "../SkimDecisionMultiFilter.h"
#include "../FileCutFlowSvc.h"
#include "../CutFlowSvc.h"
#include "EventBookkeeperTools/BookkeeperTool.h"
#include "../EventCounterAlg.h"

DECLARE_ALGORITHM_FACTORY( myCppFilterTest )
DECLARE_ALGORITHM_FACTORY( SkimDecisionMultiFilter )
DECLARE_ALGORITHM_FACTORY( EventCounterAlg )
DECLARE_TOOL_FACTORY( BookkeeperTool )
DECLARE_SERVICE_FACTORY( CutFlowSvc )
DECLARE_SERVICE_FACTORY( FileCutFlowSvc )

