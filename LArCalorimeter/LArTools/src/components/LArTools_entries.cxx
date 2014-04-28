#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LArTools/LArCablingService.h"
#include "LArTools/LArMCSymTool.h"
#include "LArTools/LArHVCablingTool.h"
#include "LArTools/LArHVPathologyDbTool.h"
#include "LArTools/LArSuperCellCablingTool.h"


DECLARE_TOOL_FACTORY( LArCablingService )
DECLARE_TOOL_FACTORY( LArHVCablingTool )
DECLARE_TOOL_FACTORY( LArMCSymTool )
DECLARE_TOOL_FACTORY( LArHVPathologyDbTool )
DECLARE_TOOL_FACTORY( LArSuperCellCablingTool )

DECLARE_FACTORY_ENTRIES(LArTools) {
  DECLARE_TOOL( LArCablingService )
  DECLARE_TOOL( LArHVCablingTool )
  DECLARE_TOOL( LArMCSymTool )
  DECLARE_TOOL( LArHVPathologyDbTool )
  DECLARE_TOOL( LArSuperCellCablingTool )
}

