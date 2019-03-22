#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TriggerMatchingTool/MatchingTool.h"
#include "TriggerMatchingTool/IParticleRetrievalTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trig, MatchingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, IParticleRetrievalTool )

#include "../TestMatchingToolAlg.h"
DECLARE_ALGORITHM_FACTORY( TestMatchingToolAlg )



DECLARE_FACTORY_ENTRIES( TriggerMatchingTool ) {
  DECLARE_ALGORITHM( TestMatchingToolAlg );
  DECLARE_NAMESPACE_TOOL(Trig, MatchingTool)
  DECLARE_NAMESPACE_TOOL(Trig, IParticleRetrievalTool )
}
