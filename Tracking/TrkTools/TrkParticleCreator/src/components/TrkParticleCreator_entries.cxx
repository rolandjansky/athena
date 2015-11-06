#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkParticleCreator/TrackParticleCreatorTool.h"

using namespace Trk ;

DECLARE_TOOL_FACTORY( TrackParticleCreatorTool )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkParticleCreator )
{
    DECLARE_TOOL( TrackParticleCreatorTool )
}
