#include "../xAODConfigSvc.h"
#include "../xAODMenuWriter.h"
#include "../xAODMenuReader.h"
#include "TrigConfxAOD/xAODConfigTool.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( TrigConf, xAODConfigSvc )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( TrigConf, xAODMenuWriter )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( TrigConf, xAODMenuReader )
DECLARE_NAMESPACE_TOOL_FACTORY( TrigConf, xAODConfigTool )

