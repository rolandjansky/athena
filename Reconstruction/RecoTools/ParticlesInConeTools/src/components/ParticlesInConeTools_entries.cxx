#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrackParticlesInConeTool.h"
#include "../TruthParticlesInConeTool.h"
#include "../CaloClustersInConeTool.h"

using namespace xAOD;

DECLARE_TOOL_FACTORY( TrackParticlesInConeTool )
DECLARE_TOOL_FACTORY( TruthParticlesInConeTool )
DECLARE_TOOL_FACTORY( CaloClustersInConeTool )

DECLARE_FACTORY_ENTRIES( ParticlesInConeTools )
{
  DECLARE_TOOL( TrackParticlesInConeTool )
  DECLARE_TOOL( TruthParticlesInConeTool )
  DECLARE_TOOL( CaloClustersInConeTool )
}

