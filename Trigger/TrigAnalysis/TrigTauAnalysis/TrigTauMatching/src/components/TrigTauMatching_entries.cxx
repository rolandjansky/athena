
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "TrigTauMatching/TrigTauMatching.h"
#include "../TrigTauMatching_example.h"

DECLARE_NAMESPACE_TOOL_FACTORY(Trig, TrigTauMatchingTool)

DECLARE_ALGORITHM_FACTORY(TrigTauMatching_example)

DECLARE_FACTORY_ENTRIES(TrigTauMatchingTool)
{
  DECLARE_NAMESPACE_TOOL(Trig, TrigTauMatchingTool)
  DECLARE_ALGORITHM(TrigTauMatching_example)
}
