#include "EventTagUtils/RawInfoSummaryForTagTool.h"
#include "EventTagUtils/EventInfoTagTool.h"
#include "EventTagUtils/GlobalEventTagTool.h"
#include "EventTagUtils/GlobalTriggerTagTool.h"
#include "EventTagUtils/RegistrationStreamTrig.h"



#include "GaudiKernel/DeclareFactoryEntries.h"
 
DECLARE_TOOL_FACTORY( RawInfoSummaryForTagTool )
DECLARE_TOOL_FACTORY( EventInfoTagTool )
DECLARE_TOOL_FACTORY( GlobalEventTagTool )
DECLARE_TOOL_FACTORY( GlobalTriggerTagTool )
DECLARE_ALGORITHM_FACTORY( RegistrationStreamTrig )
 
DECLARE_FACTORY_ENTRIES( EventTagUtils ) {
  DECLARE_TOOL( RawInfoSummaryForTagTool )
  DECLARE_TOOL( EventInfoTagTool )
  DECLARE_TOOL( GlobalEventTagTool )
  DECLARE_TOOL( GlobalTriggerTagTool )
  DECLARE_ALGORITHM( RegistrationStreamTrig )
}



