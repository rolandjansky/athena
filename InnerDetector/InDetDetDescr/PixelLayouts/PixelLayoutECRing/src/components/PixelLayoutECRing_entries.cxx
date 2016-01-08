#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PixelLayoutECRing/GeoPixelEndcapECRingTool.h"
#include "PixelLayoutECRing/GeoPixelLayerECRingTool.h"

DECLARE_TOOL_FACTORY(GeoPixelEndcapECRingTool)
DECLARE_TOOL_FACTORY(GeoPixelLayerECRingTool)
  
DECLARE_FACTORY_ENTRIES(PixelLayoutECRing) {
  DECLARE_TOOL(GeoPixelEndcapECRingTool)
  DECLARE_TOOL(GeoPixelLayerECRingTool)
}
