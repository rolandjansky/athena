#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkSM/PhotonVertexSelectionWrapper.h"
#include "DerivationFrameworkSM/LeptonVertexFitting.h"
#include "DerivationFrameworkSM/ParticlePtThinningTool.h"
using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( PhotonVertexSelectionWrapper )
DECLARE_TOOL_FACTORY( LeptonVertexFitting )
DECLARE_TOOL_FACTORY( ParticlePtThinningTool )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkSM ) {
    DECLARE_TOOL( PhotonVertexSelectionWrapper )
    DECLARE_TOOL( LeptonVertexFitting )
    DECLARE_TOOL( ParticlePtThinningTool )
}
  
