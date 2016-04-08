#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetParticleCreation/TrackParticleCreator.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TrackParticleCreator )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( InDetParticleCreation )
{
    DECLARE_ALGORITHM( TrackParticleCreator )
}
