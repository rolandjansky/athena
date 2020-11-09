

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Package include(s):
#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetUncertainties/FFJetSmearingTool.h"

DECLARE_TOOL_FACTORY(JetUncertaintiesTool)
DECLARE_NAMESPACE_TOOL(CP, FFJetSmearingTool)

DECLARE_FACTORY_ENTRIES(JetUncertainties)
{
    DECLARE_TOOL(JetUncertaintiesTool)
    DECLARE_NAMESPACE_TOOL(CP, FFJetSmearingTool)
}
