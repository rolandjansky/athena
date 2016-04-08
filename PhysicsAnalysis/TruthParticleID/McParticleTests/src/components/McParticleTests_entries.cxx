#include "../McAodSymLinkTests.h"
#include "../McAodMcTopAna.h"
#include "../McAodMcTopAna_solution.h"

#include "McParticleKernel/IMcVtxFilterTool.h"
#include "McParticleKernel/ITruthParticleCnvTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
  
DECLARE_ALGORITHM_FACTORY( McAodSymLinkTests  )
using namespace McAod;
DECLARE_ALGORITHM_FACTORY( McTopAna )
DECLARE_ALGORITHM_FACTORY( McTopAnaSolution )

DECLARE_FACTORY_ENTRIES( McParticleTests ) {
  DECLARE_ALGORITHM( McAodSymLinkTests )
  DECLARE_ALGORITHM( McTopAna )
  DECLARE_ALGORITHM( McTopAnaSolution )
}
