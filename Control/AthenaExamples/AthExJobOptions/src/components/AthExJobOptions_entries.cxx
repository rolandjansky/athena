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


DECLARE_COMPONENT( NoopAlgorithm )
DECLARE_COMPONENT( TopAlgorithm )
DECLARE_COMPONENT( SubAlgorithm )
DECLARE_COMPONENT( TestSvcHandleArrayAlg )
DECLARE_COMPONENT( ConcreteTool )
DECLARE_COMPONENT( ToolUsingTool )
DECLARE_COMPONENT( ToolSpaceTemplatedToolDouble )
DECLARE_COMPONENT( ExampleEvtLoopPreSelectTool )
DECLARE_COMPONENT( ConcreteSvc )
DECLARE_COMPONENT( AnotherConcreteSvc )

