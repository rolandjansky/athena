#include "AthenaMonitoring/AthenaMon.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"
#include "AthenaMonitoring/DQFilledBunchFilterTool.h"
#include "AthenaMonitoring/DQDummyFilterTool.h"
#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/TriggerTranslatorSimple.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(AthenaMon)
DECLARE_ALGORITHM_FACTORY(AthenaMonManager)
DECLARE_TOOL_FACTORY(ManagedMonitorToolTest)
DECLARE_TOOL_FACTORY(DQAtlasReadyFilterTool)
DECLARE_TOOL_FACTORY(DQFilledBunchFilterTool)
DECLARE_TOOL_FACTORY(DQDummyFilterTool)
DECLARE_TOOL_FACTORY(DQBadLBFilterTool)
DECLARE_TOOL_FACTORY(TriggerTranslatorToolSimple)

DECLARE_FACTORY_ENTRIES(AthenaMonitoring) {
  DECLARE_ALGORITHM(AthenaMon)
  DECLARE_ALGORITHM(AthenaMonManager)
  DECLARE_ALGTOOL(ManagedMonitorToolTest)
  DECLARE_ALGTOOL(DQAtlasReadyFilterTool)
  DECLARE_ALGTOOL(DQFilledBunchFilterTool)
  DECLARE_ALGTOOL(DQDummyFilterTool)
  DECLARE_ALGTOOL(DQBadLBFilterTool)
  DECLARE_ALGTOOL(TriggerTranslatorToolSimple)
}
