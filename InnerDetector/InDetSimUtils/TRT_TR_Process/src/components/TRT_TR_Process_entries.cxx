#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TRTPhysicsTool.h"

DECLARE_TOOL_FACTORY( TRTPhysicsTool )


DECLARE_FACTORY_ENTRIES( TRT_TR_Process ) {
  DECLARE_TOOL( TRTPhysicsTool )
 
}
