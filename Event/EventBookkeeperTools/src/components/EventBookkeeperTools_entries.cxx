#include "GaudiKernel/DeclareFactoryEntries.h"

#include "EventBookkeeperTools/myCppFilterTest.h"
#include "../SkimDecisionMultiFilter.h"
#include "../CutFlowSvc.h"
#include "EventBookkeeperTools/BookkeeperTool.h"
#include "../EventSelectorCounterTool.h"
#include "../EventCounterAlg.h"

DECLARE_ALGORITHM_FACTORY( myCppFilterTest )
DECLARE_ALGORITHM_FACTORY( SkimDecisionMultiFilter )
DECLARE_ALGORITHM_FACTORY( EventCounterAlg )
DECLARE_TOOL_FACTORY( EventSelectorCounterTool )
DECLARE_TOOL_FACTORY( BookkeeperTool )
DECLARE_SERVICE_FACTORY( CutFlowSvc )

DECLARE_FACTORY_ENTRIES(EventBookkeeperTools) {
  DECLARE_ALGORITHM( myCppFilterTest );
  DECLARE_ALGORITHM( SkimDecisionMultiFilter );
  DECLARE_ALGORITHM( EventCounterAlg );
  DECLARE_ALGTOOL( EventSelectorCounterTool );
  DECLARE_ALGTOOL( BookkeeperTool );
  DECLARE_SERVICE( CutFlowSvc );
}
