#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramToolV1.h"
#include "../TrigT1CaloMonErrorTool.h"
#include "../TrigT1CaloMonErrorToolV1.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY(LVL1, TrigT1CaloLWHistogramTool )
DECLARE_NAMESPACE_TOOL_FACTORY(LVL1, TrigT1CaloMonErrorTool )
DECLARE_NAMESPACE_TOOL_FACTORY(LVL1, TrigT1CaloLWHistogramToolV1 )
DECLARE_NAMESPACE_TOOL_FACTORY(LVL1, TrigT1CaloMonErrorToolV1 )

DECLARE_FACTORY_ENTRIES(TrigT1CaloMonitoringTools) {
  DECLARE_NAMESPACE_TOOL(LVL1, TrigT1CaloLWHistogramTool );
  DECLARE_NAMESPACE_TOOL(LVL1, TrigT1CaloLWHistogramToolV1 );
  DECLARE_NAMESPACE_TOOL(LVL1, TrigT1CaloMonErrorTool );
  DECLARE_NAMESPACE_TOOL(LVL1, TrigT1CaloMonErrorToolV1 );

}