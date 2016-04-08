#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LUCID_Monitoring/LUCID_HitsMoniTool.h"
DECLARE_TOOL_FACTORY(LUCID_HitsMoniTool)

DECLARE_FACTORY_ENTRIES(LUCID_Monitoring) {
  DECLARE_ALGTOOL(LUCID_HitsMoniTool);
}

