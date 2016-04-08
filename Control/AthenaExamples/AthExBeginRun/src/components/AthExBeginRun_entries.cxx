#include "../BeginRunAlg.h"
#include "../BeginRunTool.h"
#include "../AthExCreateCondAlg.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( BeginRunAlg )
DECLARE_ALGORITHM_FACTORY( AthExCreateCondAlg )
DECLARE_TOOL_FACTORY( BeginRunTool )

DECLARE_FACTORY_ENTRIES(AthExHelloWorld) {
    DECLARE_ALGORITHM( BeginRunAlg )
    DECLARE_ALGORITHM( AthExCreateCondAlg )
    DECLARE_TOOL( BeginRunTool )
}
