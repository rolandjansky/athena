#include "GaudiKernel/DeclareFactoryEntries.h"
#include "EndcapRingRef/GeoPixelEndcapECRingRefTool.h"
#include "EndcapRingRef/GeoPixelLayerECRingRefTool.h"

DECLARE_TOOL_FACTORY(GeoPixelEndcapECRingRefTool)
DECLARE_TOOL_FACTORY(GeoPixelLayerECRingRefTool)
  
DECLARE_FACTORY_ENTRIES(PixelLayoutECRingRef) {
  DECLARE_TOOL(GeoPixelEndcapECRingRefTool)
  DECLARE_TOOL(GeoPixelLayerECRingRefTool)
}
