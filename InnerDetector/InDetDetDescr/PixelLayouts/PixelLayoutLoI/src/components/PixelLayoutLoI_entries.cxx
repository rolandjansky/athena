#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PixelLayoutLoI/GeoPixelEnvelopeLoITool.h"
#include "PixelLayoutLoI/GeoPixelBarrelLoITool.h"
#include "PixelLayoutLoI/GeoPixelEndcapLoITool.h"

DECLARE_TOOL_FACTORY(GeoPixelEnvelopeLoITool)
DECLARE_TOOL_FACTORY(GeoPixelBarrelLoITool)
DECLARE_TOOL_FACTORY(GeoPixelEndcapLoITool)
  
DECLARE_FACTORY_ENTRIES(PixelLayoutLoI) {
  DECLARE_TOOL(GeoPixelEnvelopeLoITool)
  DECLARE_TOOL(GeoPixelBarrelLoITool)
  DECLARE_TOOL(GeoPixelEndcapLoITool)
}
