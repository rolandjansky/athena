// AsgExampleTools_entries.cxx

#include "../AsgExampleAlgorithm.h"
#include "AsgExampleTools/AsgHelloTool.h"

#include <AsgExampleTools/UnitTestTool1.h>
#include <AsgExampleTools/UnitTestTool1A.h>
#include <AsgExampleTools/UnitTestTool2.h>
#include <AsgExampleTools/UnitTestTool3.h>

DECLARE_TOOL_FACTORY(AsgHelloTool)

DECLARE_ALGORITHM_FACTORY(AsgExampleAlgorithm)

DECLARE_NAMESPACE_TOOL_FACTORY( asg, UnitTestTool1 )
DECLARE_NAMESPACE_TOOL_FACTORY( asg, UnitTestTool1A )
DECLARE_NAMESPACE_TOOL_FACTORY( asg, UnitTestTool2 )
DECLARE_NAMESPACE_TOOL_FACTORY( asg, UnitTestTool3 )

