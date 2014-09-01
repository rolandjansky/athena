#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_HepMC_Tools/GenEventValidityChecker.h"
#include "ISF_HepMC_Tools/GenEventVertexPositioner.h"
#include "ISF_HepMC_Tools/VertexBeamCondPositioner.h"
#include "ISF_HepMC_Tools/LongBeamspotVertexPositioner.h"
#include "ISF_HepMC_Tools/VertexPositionFromFile.h"
#include "ISF_HepMC_Tools/GenEventStackFiller.h"
#include "ISF_HepMC_Tools/GenParticleGenericFilter.h"
#include "ISF_HepMC_Tools/GenParticleFinalStateFilter.h"
#include "ISF_HepMC_Tools/GenParticlePositionFilter.h"
#include "ISF_HepMC_Tools/GenericTruthStrategy.h"
#include "ISF_HepMC_Tools/CylinderVolumeTruthStrategy.h"
#include "ISF_HepMC_Tools/ValidationTruthStrategy.h"


DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenEventValidityChecker      )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenEventVertexPositioner     )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , VertexBeamCondPositioner     )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , LongBeamspotVertexPositioner )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , VertexPositionFromFile       )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenEventStackFiller          )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleGenericFilter     )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleFinalStateFilter  )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticlePositionFilter    )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenericTruthStrategy         )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , CylinderVolumeTruthStrategy  )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , ValidationTruthStrategy      )

DECLARE_FACTORY_ENTRIES( ISF_HepMC_Tools ) {
  DECLARE_NAMESPACE_TOOL( ISF ,  GenEventValidityChecker      )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenEventVertexPositioner     )
  DECLARE_NAMESPACE_TOOL( ISF ,  VertexBeamCondPositioner     )
  DECLARE_NAMESPACE_TOOL( ISF ,  LongBeamspotVertexPositioner )
  DECLARE_NAMESPACE_TOOL( ISF ,  VertexPositionFromFile       )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenEventStackFiller          )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenParticleGenericFilter     )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenParticleFinalStateFilter  )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenParticlePositionFilter    )
  DECLARE_NAMESPACE_TOOL( ISF ,  GenericTruthStrategy         )
  DECLARE_NAMESPACE_TOOL( ISF ,  CylinderVolumeTruthStrategy  )
  DECLARE_NAMESPACE_TOOL( ISF ,  ValidationTruthStrategy      )
}   
