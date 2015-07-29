
#include "PixelGeoModel/PixelDetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PixelGeoModel/IBLParameterSvc.h"

DECLARE_TOOL_FACTORY(PixelDetectorTool)
DECLARE_SERVICE_FACTORY(IBLParameterSvc)

DECLARE_FACTORY_ENTRIES(PixelGeoModel) {
    DECLARE_ALGTOOL  ( PixelDetectorTool )
    DECLARE_SERVICE(IBLParameterSvc) 
}
