#include "../PixelFillCablingData_Final.h"
#include "../PixelFillCablingData.h"
#include "../PixelCablingSvc.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( PixelFillCablingData_Final )
DECLARE_TOOL_FACTORY( PixelFillCablingData )
DECLARE_SERVICE_FACTORY(PixelCablingSvc)

DECLARE_FACTORY_ENTRIES( PixelCabling ) 
{
  DECLARE_TOOL( PixelFillCablingData )
  DECLARE_TOOL( PixelFillCablingData_Final )
  DECLARE_SERVICE(PixelCablingSvc)
}
