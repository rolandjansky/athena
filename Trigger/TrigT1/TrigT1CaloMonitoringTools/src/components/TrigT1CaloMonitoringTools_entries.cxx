#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloMonErrorTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(TrigT1CaloLWHistogramTool )
DECLARE_TOOL_FACTORY(TrigT1CaloMonErrorTool )

DECLARE_FACTORY_ENTRIES(TrigT1CaloMonitoringTools) {
  DECLARE_ALGTOOL(TrigT1CaloLWHistogramTool );
  DECLARE_ALGTOOL(TrigT1CaloMonErrorTool );
}

