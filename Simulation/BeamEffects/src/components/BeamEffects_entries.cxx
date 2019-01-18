#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../GenEventValidityChecker.h"
#include "../ZeroLifetimePositioner.h"
#include "../GenEventVertexPositioner.h"
#include "../VertexBeamCondPositioner.h"
#include "../LongBeamspotVertexPositioner.h"
#include "../CrabKissingVertexPositioner.h"
#include "../VertexPositionFromFile.h"
#include "../GenEventBeamEffectBooster.h"
#include "../GenEventRotator.h"
#include "../BeamEffectsAlg.h"
DECLARE_NAMESPACE_SERVICE_FACTORY( Simulation , ZeroLifetimePositioner    )
DECLARE_NAMESPACE_TOOL_FACTORY( Simulation , GenEventValidityChecker      )
DECLARE_NAMESPACE_TOOL_FACTORY( Simulation , GenEventVertexPositioner     )
DECLARE_NAMESPACE_TOOL_FACTORY( Simulation , VertexBeamCondPositioner     )
DECLARE_NAMESPACE_TOOL_FACTORY( Simulation , LongBeamspotVertexPositioner )
DECLARE_NAMESPACE_TOOL_FACTORY( Simulation , CrabKissingVertexPositioner  )
DECLARE_NAMESPACE_TOOL_FACTORY( Simulation , VertexPositionFromFile       )
DECLARE_NAMESPACE_TOOL_FACTORY( Simulation , GenEventBeamEffectBooster    )
DECLARE_NAMESPACE_TOOL_FACTORY( Simulation , GenEventRotator              )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Simulation , BeamEffectsAlg          )
