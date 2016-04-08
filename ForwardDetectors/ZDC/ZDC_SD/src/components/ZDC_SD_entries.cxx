#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../ZDC_PixelSDTool.h"
#include "../ZDC_StripSDTool.h"

DECLARE_TOOL_FACTORY( ZDC_PixelSDTool )
DECLARE_TOOL_FACTORY( ZDC_StripSDTool )

DECLARE_FACTORY_ENTRIES( ZDC_SD ){
  DECLARE_TOOL( ZDC_PixelSDTool );
  DECLARE_TOOL( ZDC_StripSDTool );
}
