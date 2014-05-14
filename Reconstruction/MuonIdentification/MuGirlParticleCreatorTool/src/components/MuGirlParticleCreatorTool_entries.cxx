#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuGirlParticleCreatorTool/MuGirlParticleCreatorTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( MuGirlNS, MuGirlParticleCreatorTool )

DECLARE_FACTORY_ENTRIES( MuGirlParticleCreatorTool )
{
	DECLARE_NAMESPACE_TOOL( MuGirlNS, MuGirlParticleCreatorTool )
}

