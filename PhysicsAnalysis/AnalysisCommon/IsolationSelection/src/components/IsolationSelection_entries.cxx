// Local include(s):
#include "IsolationSelection/IsolationSelectionTool.h"
#include "../TestIsolationAthenaAlg.h"

#include "IsolationSelection/IsolationCloseByCorrectionTool.h"
#include "../TestIsolationCloseByCorrAthenaAlg.h"

#include "IsolationSelection/IsolationLowPtPLVTool.h"
using namespace xAOD;

DECLARE_COMPONENT( CP::IsolationSelectionTool )
DECLARE_COMPONENT( CP::TestIsolationAthenaAlg )

DECLARE_COMPONENT( CP::IsolationCloseByCorrectionTool )
DECLARE_COMPONENT( CP::TestIsolationCloseByCorrAthenaAlg )

DECLARE_COMPONENT (CP::IsolationLowPtPLVTool )
