#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../G4StepLimitationTool.h"


DECLARE_TOOL_FACTORY( G4StepLimitationTool )


DECLARE_FACTORY_ENTRIES( G4StepLimitation ) {
  DECLARE_TOOL( G4StepLimitationTool )
}
