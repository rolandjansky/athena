#include "FastChainPileup/MultiPy8Pileup.h"
#include "FastChainPileup/MultiParticleGunPileup.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( MultiPy8Pileup )
DECLARE_ALGORITHM_FACTORY( MultiParticleGunPileup )

DECLARE_FACTORY_ENTRIES( FastChainPileup )
{
  DECLARE_ALGORITHM( MultiPy8Pileup )
  DECLARE_ALGORITHM( MultiParticleGunPileup )
}
