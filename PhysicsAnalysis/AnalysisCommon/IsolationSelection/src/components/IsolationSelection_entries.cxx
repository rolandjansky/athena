#include "IsolationSelection/IsolationSelectionTool.h"
#include "../TestIsolationAthenaAlg.h"

#include "IsolationSelection/IsolationCloseByCorrectionTool.h"
#include "../TestIsolationCloseByCorrAthenaAlg.h"

using namespace xAOD;

DECLARE_NAMESPACE_TOOL_FACTORY(CP, IsolationSelectionTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(CP, TestIsolationAthenaAlg)

DECLARE_NAMESPACE_TOOL_FACTORY(CP, IsolationCloseByCorrectionTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(CP, TestIsolationCloseByCorrAthenaAlg)

