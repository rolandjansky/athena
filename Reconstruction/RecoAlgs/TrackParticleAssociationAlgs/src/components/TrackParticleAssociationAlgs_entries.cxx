#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrackParticleCellAssociationAlg.h"
#include "../TrackParticleClusterAssociationAlg.h"

DECLARE_ALGORITHM_FACTORY( TrackParticleCellAssociationAlg )
DECLARE_ALGORITHM_FACTORY( TrackParticleClusterAssociationAlg )

DECLARE_FACTORY_ENTRIES( TrackParticleAssociationAlgs )
{
  DECLARE_ALGORITHM( TrackParticleCellAssociationAlg )
  DECLARE_ALGORITHM( TrackParticleClusterAssociationAlg )
}
