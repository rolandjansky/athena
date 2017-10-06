#include "AthExJobOptions/NoopAlgorithm.h"
#include "AthExJobOptions/TopAlgorithm.h"
#include "AthExJobOptions/SubAlgorithm.h"
#include "AthExJobOptions/TestSvcHandleArrayAlg.h"
#include "AthExJobOptions/ConcreteTool.h"
#include "AthExJobOptions/ToolUsingTool.h"
#include "AthExJobOptions/TemplatedTool.h"
#include "AthExJobOptions/ConcreteSvc.h"
#include "AthExJobOptions/AnotherConcreteSvc.h"
#include "AthExJobOptions/ExampleEvtLoopPreSelectTool.h"


typedef ToolSpace::TemplatedTool< double > ToolSpaceTemplatedToolDouble;


DECLARE_ALGORITHM_FACTORY( NoopAlgorithm )
DECLARE_ALGORITHM_FACTORY( TopAlgorithm )
DECLARE_ALGORITHM_FACTORY( SubAlgorithm )
DECLARE_ALGORITHM_FACTORY( TestSvcHandleArrayAlg )
DECLARE_TOOL_FACTORY( ConcreteTool )
DECLARE_TOOL_FACTORY( ToolUsingTool )
DECLARE_TOOL_FACTORY( ToolSpaceTemplatedToolDouble )
DECLARE_TOOL_FACTORY( ExampleEvtLoopPreSelectTool )
DECLARE_SERVICE_FACTORY( ConcreteSvc )
DECLARE_SERVICE_FACTORY( AnotherConcreteSvc )

