#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../GenEventStackFiller.h"
#include "../GenParticleGenericFilter.h"
#include "../GenParticleFinalStateFilter.h"
#include "../GenParticlePositionFilter.h"
#include "../GenericTruthStrategy.h"
#include "../CylinderVolumeTruthStrategy.h"
#include "../ValidationTruthStrategy.h"
#include "../LLPTruthStrategy.h"
#include "../GenParticleSimWhiteList.h"
#include "../GenParticleInteractingFilter.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenEventStackFiller          )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleGenericFilter     )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleFinalStateFilter  )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticlePositionFilter    )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenericTruthStrategy         )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , CylinderVolumeTruthStrategy  )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , ValidationTruthStrategy      )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , LLPTruthStrategy             )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleSimWhiteList      )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleInteractingFilter )
