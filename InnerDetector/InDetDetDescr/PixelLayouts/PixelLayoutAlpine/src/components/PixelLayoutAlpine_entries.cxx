#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PixelLayoutAlpine/GeoPixelEnvelopeAlpineTool.h"
#include "PixelLayoutAlpine/GeoPixelBarrelAlpineTool.h"

DECLARE_TOOL_FACTORY(GeoPixelEnvelopeAlpineTool)
DECLARE_TOOL_FACTORY(GeoPixelBarrelAlpineTool)
  
DECLARE_FACTORY_ENTRIES(PixelLayoutAlpine) {
  DECLARE_TOOL(GeoPixelEnvelopeAlpineTool)
  DECLARE_TOOL(GeoPixelBarrelAlpineTool)
}
