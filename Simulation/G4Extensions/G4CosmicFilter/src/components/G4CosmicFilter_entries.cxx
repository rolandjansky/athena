#include "GaudiKernel/DeclareFactoryEntries.h"

#include "G4CosmicFilter/G4CosmicFilter.h"
#include "G4CosmicFilter/G4CosmicAndFilter.h"
#include "G4CosmicFilter/G4CosmicOrFilter.h"

#include "G4CosmicFilter/G4CosmicFilterTool.h"
#include "G4CosmicFilter/G4CosmicAndFilterTool.h"
#include "G4CosmicFilter/G4CosmicOrFilterTool.h"


DECLARE_TOOL_FACTORY( G4UA::G4CosmicFilterTool )
DECLARE_TOOL_FACTORY( G4UA::G4CosmicAndFilterTool )
DECLARE_TOOL_FACTORY( G4UA::G4CosmicOrFilterTool )

DECLARE_FACTORY_ENTRIES( G4CosmicFilter ) {

  DECLARE_TOOL( G4UA::G4CosmicFilterTool )
  DECLARE_TOOL( G4UA::G4CosmicAndFilterTool )
  DECLARE_TOOL( G4UA::G4CosmicOrFilterTool )

}
