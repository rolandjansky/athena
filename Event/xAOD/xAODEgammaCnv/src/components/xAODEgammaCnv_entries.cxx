


// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../ElectronCnvAlg.h"
#include "../PhotonCnvAlg.h"
#include "../ElectronCnvTool.h"
#include "../PhotonCnvTool.h"
#include "../GSFTrackParticleMapCnvAlg.h"
#include "../ElectronReader.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, ElectronCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, PhotonCnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, ElectronCnvTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, PhotonCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, GSFTrackParticleMapCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, ElectronReader )
	
DECLARE_FACTORY_ENTRIES( xAODEgammaCnv ) {
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, ElectronCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, PhotonCnvAlg )
   DECLARE_NAMESPACE_TOOL( xAODMaker, ElectronCnvTool )
   DECLARE_NAMESPACE_TOOL( xAODMaker, PhotonCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, GSFTrackParticleMapCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODReader, ElectronReader )

}
