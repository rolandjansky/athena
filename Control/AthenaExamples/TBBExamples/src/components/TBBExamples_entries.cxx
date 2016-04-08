#include "../HelloGraphAlg.h"
#include "../HelloParForAlg.h"
#include "../HelloPipeAlg.h"
#include "../TBBTool.h"
//#include "../HelloSvc.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( HelloGraphAlg )
DECLARE_ALGORITHM_FACTORY( HelloParForAlg )
DECLARE_ALGORITHM_FACTORY( HelloPipeAlg )
DECLARE_TOOL_FACTORY( TBBTool )
//DECLARE_SERVICE_FACTORY( HelloSvc )

DECLARE_FACTORY_ENTRIES(TBBExamples) {
    DECLARE_ALGORITHM( HelloGraphAlg )
    DECLARE_ALGORITHM( HelloParForAlg )
    DECLARE_ALGORITHM( HelloPipeAlg )
    DECLARE_TOOL( TBBTool )
      //    DECLARE_SERVICE( HelloSvc )
}
