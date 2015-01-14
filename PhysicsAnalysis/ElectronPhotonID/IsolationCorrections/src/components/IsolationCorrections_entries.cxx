// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "IsolationCorrections/IsolationCorrectionTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CP, IsolationCorrectionTool )

DECLARE_FACTORY_ENTRIES( IsolationCorrections) {
  DECLARE_NAMESPACE_TOOL( CP, IsolationCorrectionTool )
}
