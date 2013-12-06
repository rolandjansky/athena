#include "TrigT1Monitoring/L1CaloCTPMon.h"
#include "TrigT1Monitoring/L1CaloLevel2Mon.h"
#include "TrigT1Monitoring/CalorimeterL1CaloMon.h"
#include "TrigT1Monitoring/L1CaloHVScalesMon.h"
#include "TrigT1Monitoring/L1CaloPMTScoresMon.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(L1CaloCTPMon)
DECLARE_TOOL_FACTORY(L1CaloLevel2Mon)
DECLARE_TOOL_FACTORY(CalorimeterL1CaloMon)
DECLARE_TOOL_FACTORY(L1CaloHVScalesMon)
DECLARE_TOOL_FACTORY(L1CaloPMTScoresMon)

DECLARE_FACTORY_ENTRIES(TrigT1Monitoring) {
  DECLARE_ALGTOOL(L1CaloCTPMon)
  DECLARE_ALGTOOL(L1CaloLevel2Mon)
  DECLARE_ALGTOOL(CalorimeterL1CaloMon)
  DECLARE_ALGTOOL(L1CaloHVScalesMon)
  DECLARE_ALGTOOL(L1CaloPMTScoresMon)
}

