#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../CylinderVolumeTruthStrategy.h"
#include "../GenericTruthStrategy.h"
#include "../GenParticleFinalStateFilter.h"
#include "../GenParticleGenericFilter.h"
#include "../GenParticleInteractingFilter.h"
#include "../GenParticleLifetimeFilter.h"
#include "../GenParticlePositionFilter.h"
#include "../GenParticleSimWhiteList.h"
#include "../GenParticleSimQuasiStableFilter.h"
#include "../LLPTruthStrategy.h"
#include "../ValidationTruthStrategy.h"
#include "../KeepChildrenTruthStrategy.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ISF , CylinderVolumeTruthStrategy  )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenericTruthStrategy         )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleFinalStateFilter  )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleGenericFilter     )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleInteractingFilter )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleLifetimeFilter    )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticlePositionFilter    )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleSimWhiteList      )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , GenParticleSimQuasiStableFilter)
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , LLPTruthStrategy             )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , ValidationTruthStrategy      )
DECLARE_NAMESPACE_TOOL_FACTORY( ISF , KeepChildrenTruthStrategy    )
