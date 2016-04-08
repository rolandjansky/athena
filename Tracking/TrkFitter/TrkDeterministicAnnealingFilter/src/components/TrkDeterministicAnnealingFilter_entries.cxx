
#include "TrkDeterministicAnnealingFilter/DeterministicAnnealingFilter.h"
#include "TrkDeterministicAnnealingFilter/DAF_WeightCalculator.h"
#include "TrkDeterministicAnnealingFilter/DAF_SimpleWeightCalculator.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

// using namespace Trk;

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, DeterministicAnnealingFilter )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, DAF_WeightCalculator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, DAF_SimpleWeightCalculator )

DECLARE_FACTORY_ENTRIES(TrkDeterministicAnnealingFilter)
{ // in this package name these methods are registered 
  DECLARE_NAMESPACE_TOOL(Trk, DeterministicAnnealingFilter)
  DECLARE_NAMESPACE_TOOL(Trk, DAF_WeightCalculator)
  DECLARE_NAMESPACE_TOOL(Trk, DAF_SimpleWeightCalculator)
}

