
#include "PixelModuleTool/PixelModuleBuilder.h"
#include "PixelModuleTool/PixelDesignBuilder.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY(PixelModuleBuilder)
DECLARE_SERVICE_FACTORY(PixelDesignBuilder)

DECLARE_FACTORY_ENTRIES(PixelModuleTool) 
{
   DECLARE_SERVICE(PixelModuleBuilder)
   DECLARE_SERVICE(PixelDesignBuilder)
}




