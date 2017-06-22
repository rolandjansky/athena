 #include "GaudiKernel/DeclareFactoryEntries.h"

#include "../RHadronsPhysicsTool.h"
#include "../SG_StepNtupleTool.h"

DECLARE_TOOL_FACTORY( RHadronsPhysicsTool )
DECLARE_TOOL_FACTORY( G4UA::SG_StepNtupleTool )


DECLARE_FACTORY_ENTRIES( RHadrons ) {

  DECLARE_TOOL( RHadronsPhysicsTool )
  DECLARE_TOOL( G4UA::SG_StepNtupleTool )

}
