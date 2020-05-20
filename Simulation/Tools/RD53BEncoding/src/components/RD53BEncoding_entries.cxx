#include "GaudiKernel/DeclareFactoryEntries.h"

#include "RD53BEncoding/RD53BEncodingAlg.h"
#include "../RD53BEncodingTool.h"

DECLARE_ALGORITHM_FACTORY( RD53BEncodingAlg )
DECLARE_TOOL_FACTORY( RD53BEncodingTool )

DECLARE_FACTORY_ENTRIES( RD53BEncoding ) {
  DECLARE_ALGORITHM( RD53BEncodingAlg )
  DECLARE_TOOL( RD53BEncodingTool )
}
