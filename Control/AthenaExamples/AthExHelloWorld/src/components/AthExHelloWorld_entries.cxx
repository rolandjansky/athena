#include "../HelloAlg.h"
#include "../HelloTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( HelloAlg )
DECLARE_TOOL_FACTORY( HelloTool )

DECLARE_FACTORY_ENTRIES(AthExHelloWorld) {
    DECLARE_ALGORITHM( HelloAlg )
    DECLARE_TOOL( HelloTool )
}
