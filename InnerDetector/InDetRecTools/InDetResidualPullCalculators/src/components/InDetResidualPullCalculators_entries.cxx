#include "../SCT_ResidualPullCalculator.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, SCT_ResidualPullCalculator )

DECLARE_FACTORY_ENTRIES(InDetTrackValidation){
  DECLARE_NAMESPACE_TOOL(InDet, SCT_ResidualPullCalculator)
}

