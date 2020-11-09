#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkSM/PhotonVertexSelectionWrapper.h"
#include "DerivationFrameworkSM/LeptonVertexFitting.h"
using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( PhotonVertexSelectionWrapper )
DECLARE_TOOL_FACTORY( LeptonVertexFitting )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkSM ) {
    DECLARE_TOOL( PhotonVertexSelectionWrapper )
    DECLARE_TOOL( LeptonVertexFitting )
}
  
