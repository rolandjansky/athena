#include "IsolationTool/TrackIsolationTool.h"
#include "IsolationTool/CaloIsolationTool.h"
#include "IsolationTool/IsolationHelper.h"
#include "../IsolationTool_AthTest.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAOD, TrackIsolationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAOD, CaloIsolationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( CP, IsolationHelper )

DECLARE_ALGORITHM_FACTORY( IsolationTool_AthTest )

