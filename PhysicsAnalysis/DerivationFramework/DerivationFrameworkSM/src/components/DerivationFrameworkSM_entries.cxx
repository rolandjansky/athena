#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkSM/PhotonVertexSelectionWrapper.h"
#include "DerivationFrameworkSM/TriggerMatchingAugmentation.h"
using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( PhotonVertexSelectionWrapper )
DECLARE_TOOL_FACTORY( TriggerMatchingAugmentation  )
DECLARE_FACTORY_ENTRIES( DerivationFrameworkSM ) {
    DECLARE_TOOL( PhotonVertexSelectionWrapper )
    DECLARE_TOOL( TriggerMatchingAugmentation )
}
  
