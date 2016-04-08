#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../LucidPhysicsTool.h"

DECLARE_TOOL_FACTORY( LucidPhysicsTool )

DECLARE_FACTORY_ENTRIES( LUCID_OpProcess ) {
  DECLARE_TOOL( LucidPhysicsTool )

}
