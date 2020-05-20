#include "GaudiKernel/DeclareFactoryEntries.h"
#include "BCMPrime/GeoBCMPrimeTool.h"

DECLARE_TOOL_FACTORY(GeoBCMPrimeTool)

DECLARE_FACTORY_ENTRIES(PixelLayoutInclRef) {
  DECLARE_TOOL(GeoBCMPrimeTool)
}