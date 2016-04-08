#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../UnassociatedHitsFillerTool.h"
#include "../UnassociatedHitsGetterTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, UnassociatedHitsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, UnassociatedHitsGetterTool)

DECLARE_FACTORY_ENTRIES(MinBiasD3PDMaker) {
  DECLARE_NAMESPACE_TOOL      (D3PD, UnassociatedHitsFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, UnassociatedHitsGetterTool)
}
