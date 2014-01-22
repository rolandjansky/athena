#include "GaudiKernel/DeclareFactoryEntries.h"
#include "AFP_Digitization/AFP_DigiTop.h"
#include "AFP_Digitization/AFP_PileUpTool.h"

DECLARE_ALGORITHM_FACTORY(AFP_DigiTop)

DECLARE_TOOL_FACTORY( AFP_PileUpTool )

DECLARE_FACTORY_ENTRIES  (AFP_Digitization) 
{
  DECLARE_ALGORITHM(AFP_DigiTop)
    DECLARE_TOOL( AFP_PileUpTool )
}

