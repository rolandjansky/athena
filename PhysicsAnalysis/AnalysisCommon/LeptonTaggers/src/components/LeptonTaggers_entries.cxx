// Framework
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local
#include "../DecoratePromptLepton.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Prompt, DecoratePromptLepton )

DECLARE_FACTORY_ENTRIES( LeptonTaggers )
{
  DECLARE_NAMESPACE_ALGORITHM( Prompt, DecoratePromptLepton )
}

