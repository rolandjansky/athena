#include "DetDescrCondTools/AlignTransTool.h"
#include "../CoolStrFileSvc.h"
#include "../CoolHistSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( AlignTransTool ) 
DECLARE_SERVICE_FACTORY( CoolStrFileSvc ) 
DECLARE_SERVICE_FACTORY( CoolHistSvc ) 

DECLARE_FACTORY_ENTRIES(DetDescrCondTools) {
  DECLARE_TOOL( AlignTransTool ) 
  DECLARE_SERVICE( CoolStrFileSvc ) 
  DECLARE_SERVICE( CoolHistSvc ) 
}
