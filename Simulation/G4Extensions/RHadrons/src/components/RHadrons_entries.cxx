 #include "GaudiKernel/DeclareFactoryEntries.h"

#include "../RHadronsPhysicsTool.h"
#include "../SG_StepNtuple.h"

DECLARE_TOOL_FACTORY( RHadronsPhysicsTool )
DECLARE_TOOL_FACTORY( SG_StepNtuple )


DECLARE_FACTORY_ENTRIES( RHadrons ) {

  DECLARE_TOOL( RHadronsPhysicsTool )
  DECLARE_TOOL( SG_StepNtuple )

}
