#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PFlowUtils/RetrievePFOTool.h"


DECLARE_NAMESPACE_TOOL_FACTORY( CP, RetrievePFOTool)

DECLARE_FACTORY_ENTRIES(PFlowUtils){
  DECLARE_NAMESPACE_TOOL (CP, RetrievePFOTool)
}
