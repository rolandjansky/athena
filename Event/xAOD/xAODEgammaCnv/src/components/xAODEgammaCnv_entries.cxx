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

