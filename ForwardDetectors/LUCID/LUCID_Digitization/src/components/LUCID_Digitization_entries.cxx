#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../LUCID_Digitization.h"
#include "../LUCID_PileUpTool.h"


DECLARE_ALGORITHM_FACTORY( LUCID_DigiTop )
DECLARE_TOOL_FACTORY( LUCID_PileUpTool )
  
DECLARE_FACTORY_ENTRIES(LUCID_Digitization) {

  DECLARE_ALGORITHM(LUCID_DigiTop)
  DECLARE_ALGTOOL(LUCID_PileUpTool)

}
