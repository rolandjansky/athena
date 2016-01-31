#include "GaudiKernel/DeclareFactoryEntries.h"
#include "BarrelInclinedRef/GeoPixelEnvelopeInclRefTool.h"
#include "BarrelInclinedRef/GeoPixelBarrelInclRefTool.h"
#include "BarrelInclinedRef/GeoPixelLayerInclRefTool.h"
#include "BarrelInclinedRef/GeoPixelLayerPlanarRefTool.h"

DECLARE_TOOL_FACTORY(GeoPixelEnvelopeInclRefTool)
DECLARE_TOOL_FACTORY(GeoPixelBarrelInclRefTool)
DECLARE_TOOL_FACTORY(GeoPixelLayerInclRefTool)
DECLARE_TOOL_FACTORY(GeoPixelLayerPlanarRefTool)
  
DECLARE_FACTORY_ENTRIES(PixelLayoutInclRef) {
  DECLARE_TOOL(GeoPixelEnvelopeInclRefTool)
  DECLARE_TOOL(GeoPixelBarrelInclRefTool)
  DECLARE_TOOL(GeoPixelLayerInclRefTool)
  DECLARE_TOOL(GeoPixelLayerPlanarRefTool)
}
