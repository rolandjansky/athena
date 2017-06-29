
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TruthConverters/xAODtoHepMCTool.h"

DECLARE_TOOL_FACTORY( xAODtoHepMCTool )

DECLARE_FACTORY_ENTRIES( TruthConverters ) 
{
  DECLARE_TOOL( xAODtoHepMCTool );
}
