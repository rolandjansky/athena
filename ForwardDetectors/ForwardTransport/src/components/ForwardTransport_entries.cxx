#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../ForwardTransportModelTool.h"

DECLARE_TOOL_FACTORY( ForwardTransportModelTool )

DECLARE_FACTORY_ENTRIES( ForwardTransport ) {
  DECLARE_TOOL( ForwardTransportModelTool )
}
