#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloMonErrorTool.h"
//Run1 
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramToolV1.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(TrigT1CaloLWHistogramTool )
DECLARE_TOOL_FACTORY(TrigT1CaloMonErrorTool )
//Run1
DECLARE_TOOL_FACTORY(TrigT1CaloLWHistogramToolV1 )

DECLARE_FACTORY_ENTRIES(TrigT1CaloMonitoringTools) {
  DECLARE_ALGTOOL(TrigT1CaloLWHistogramTool );
  DECLARE_ALGTOOL(TrigT1CaloMonErrorTool );
  //Run1
  DECLARE_ALGTOOL(TrigT1CaloLWHistogramToolV1 );
}

