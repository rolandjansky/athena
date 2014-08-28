#include "../AraToolExampleAlg.h"
#include "../AraToolUsingToolAlg.h"

#include "AraToolExample/AnExampleAraToolWrapper.h"
#include "AraToolExample/AraToolUsingToolWrapper.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( AnExampleAraToolWrapper )
DECLARE_TOOL_FACTORY( AraToolUsingToolWrapper )

DECLARE_ALGORITHM_FACTORY( AraToolExampleAlg )
DECLARE_ALGORITHM_FACTORY( AraToolUsingToolAlg )

DECLARE_FACTORY_ENTRIES( AraToolExample ) {
    DECLARE_ALGORITHM( AraToolExampleAlg )
    DECLARE_ALGORITHM( AraToolUsingToolAlg )
      DECLARE_ALGTOOL( AnExampleAraToolWrapper )
      DECLARE_ALGTOOL( AraToolUsingToolWrapper )
}
