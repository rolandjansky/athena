#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../AthExUnittestAlg.h"
DECLARE_ALGORITHM_FACTORY( AthExUnittestAlg )

#include "AthExUnittest/AthExUnittestTool.h"
DECLARE_TOOL_FACTORY( AthExUnittestTool )

DECLARE_FACTORY_ENTRIES( AthExUnittest ) {
  DECLARE_TOOL( AthExUnittestTool );
  DECLARE_ALGORITHM( AthExUnittestAlg );
}
