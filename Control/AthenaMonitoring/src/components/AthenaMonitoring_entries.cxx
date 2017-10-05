#include "AthenaMonitoring/AthenaMon.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"
#include "AthenaMonitoring/DQFilledBunchFilterTool.h"
#include "AthenaMonitoring/DQDummyFilterTool.h"
#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/TriggerTranslatorSimple.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"


DECLARE_ALGORITHM_FACTORY(AthenaMon)
DECLARE_ALGORITHM_FACTORY(AthenaMonManager)
DECLARE_TOOL_FACTORY(ManagedMonitorToolTest)
DECLARE_TOOL_FACTORY(DQAtlasReadyFilterTool)
DECLARE_TOOL_FACTORY(DQFilledBunchFilterTool)
DECLARE_TOOL_FACTORY(DQDummyFilterTool)
DECLARE_TOOL_FACTORY(DQBadLBFilterTool)
DECLARE_TOOL_FACTORY(TriggerTranslatorToolSimple)
DECLARE_TOOL_FACTORY(GenericMonitoringTool)

