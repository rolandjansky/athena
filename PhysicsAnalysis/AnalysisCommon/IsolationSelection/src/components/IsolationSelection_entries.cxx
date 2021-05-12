// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "IsolationSelection/IsolationSelectionTool.h"
#include "../TestIsolationAthenaAlg.h"

#include "IsolationSelection/IsolationCloseByCorrectionTool.h"
#include "../TestIsolationCloseByCorrAthenaAlg.h"

#include "IsolationSelection/IsolationLowPtPLVTool.h"
using namespace xAOD;

DECLARE_NAMESPACE_TOOL_FACTORY(CP, IsolationSelectionTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(CP, TestIsolationAthenaAlg)

DECLARE_NAMESPACE_TOOL_FACTORY(CP, IsolationCloseByCorrectionTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(CP, TestIsolationCloseByCorrAthenaAlg)

DECLARE_NAMESPACE_TOOL_FACTORY(CP, IsolationLowPtPLVTool)

DECLARE_FACTORY_ENTRIES(IsolationSelection) {
    DECLARE_NAMESPACE_TOOL(CP, IsolationSelectionTool)
    DECLARE_NAMESPACE_ALGORITHM(CP, TestIsolationAthenaAlg)
    DECLARE_NAMESPACE_TOOL(CP, IsolationCloseByCorrectionTool)
    DECLARE_NAMESPACE_ALGORITHM(CP, TestIsolationCloseByCorrAthenaAlg)
    DECLARE_NAMESPACE_TOOL(CP, IsolationLowPtPLVTool)
}
