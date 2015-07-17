#include "../PixelFillCablingData_CoralDB.h"
#include "../PixelFillCablingData_Final.h"
#include "../PixelCablingSvc.h"
#include "../PixelConnCoolDbSvc.h"
#include "../PixelReadoutSpeedDbSvc.h"
#include "../DumpCabling.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( PixelFillCablingData_CoralDB )
DECLARE_TOOL_FACTORY( PixelFillCablingData_Final )
DECLARE_SERVICE_FACTORY(PixelCablingSvc)
DECLARE_SERVICE_FACTORY(PixelConnCoolDbSvc)
DECLARE_SERVICE_FACTORY(PixelReadoutSpeedDbSvc)
DECLARE_ALGORITHM_FACTORY(DumpCabling)

DECLARE_FACTORY_ENTRIES( PixelCabling ) 
{
  DECLARE_TOOL( PixelFillCablingData_Final )
  DECLARE_TOOL( PixelFillCablingData_CoralDB )
  DECLARE_SERVICE(PixelCablingSvc)
  DECLARE_SERVICE(PixelConnCoolDbSvc)
  DECLARE_SERVICE(PixelReadoutSpeedDbSvc)
  DECLARE_ALGORITHM(DumpCabling)
}
