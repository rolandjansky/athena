#include "../NoopAlgorithm.h"
#include "../TopAlgorithm.h"
#include "../SubAlgorithm.h"
#include "../TestSvcHandleArrayAlg.h"
#include "../ConcreteTool.h"
#include "../ToolUsingTool.h"
#include "../TemplatedTool.h"
#include "../ConcreteSvc.h"
#include "../AnotherConcreteSvc.h"
#include "../ExampleEvtLoopPreSelectTool.h"


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

