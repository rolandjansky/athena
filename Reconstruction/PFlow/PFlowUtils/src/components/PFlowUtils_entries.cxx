#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PFlowUtils/RetrievePFOTool.h"
#include "PFlowUtils/WeightPFOTool.h"
#include "../ParticleFlowEventFilter_r207.h"
#include "../PFlowMerger.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CP, RetrievePFOTool)
DECLARE_NAMESPACE_TOOL_FACTORY( CP, WeightPFOTool)
DECLARE_ALGORITHM_FACTORY(ParticleFlowEventFilter_r207)
DECLARE_ALGORITHM_FACTORY( PFlowMerger )

DECLARE_FACTORY_ENTRIES(PFlowUtils){
  DECLARE_NAMESPACE_TOOL (CP, RetrievePFOTool)
  DECLARE_NAMESPACE_TOOL (CP, WeightPFOTool)
  DECLARE_ALGORITHM(ParticleFlowEventFilter_r207)  
}
