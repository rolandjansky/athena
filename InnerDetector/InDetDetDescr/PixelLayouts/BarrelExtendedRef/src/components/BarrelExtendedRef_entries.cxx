#include "GaudiKernel/DeclareFactoryEntries.h"
#include "BarrelExtendedRef/GeoPixelEnvelopeExtRefTool.h"
#include "BarrelExtendedRef/GeoPixelBarrelExtRefTool.h"

DECLARE_TOOL_FACTORY(GeoPixelEnvelopeExtRefTool)
DECLARE_TOOL_FACTORY(GeoPixelBarrelExtRefTool)
  
DECLARE_FACTORY_ENTRIES(PixelLayoutExtRef) {
  DECLARE_TOOL(GeoPixelEnvelopeExtRefTool)
  DECLARE_TOOL(GeoPixelBarrelExtRefTool)
}
