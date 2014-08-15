#include "../AthMpEvtLoopMgr.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
  
DECLARE_SERVICE_FACTORY (AthMpEvtLoopMgr)

DECLARE_FACTORY_ENTRIES( AthenaMP ) {
  DECLARE_SERVICE (AthMpEvtLoopMgr)
}
