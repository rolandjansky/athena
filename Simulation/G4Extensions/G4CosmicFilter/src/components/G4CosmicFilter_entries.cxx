#include "GaudiKernel/DeclareFactoryEntries.h"

#include "G4CosmicFilter/G4CosmicFilter.h"
#include "G4CosmicFilter/G4CosmicAndFilter.h"
#include "G4CosmicFilter/G4CosmicOrFilter.h"


DECLARE_TOOL_FACTORY( G4CosmicFilter )
DECLARE_TOOL_FACTORY( G4CosmicAndFilter )
DECLARE_TOOL_FACTORY( G4CosmicOrFilter )

DECLARE_FACTORY_ENTRIES( G4CosmicFilter ) {
  DECLARE_TOOL( G4CosmicFilter )
  DECLARE_TOOL( G4CosmicAndFilter )
  DECLARE_TOOL( G4CosmicOrFilter )
}
