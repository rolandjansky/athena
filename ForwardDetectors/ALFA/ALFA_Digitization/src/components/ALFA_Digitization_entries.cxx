#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ALFA_Digitization/ALFA_DigiAlg.h"
#include "ALFA_Digitization/ALFA_PileUpTool.h"


DECLARE_ALGORITHM_FACTORY(ALFA_DigiAlg)
DECLARE_TOOL_FACTORY(ALFA_PileUpTool)

DECLARE_FACTORY_ENTRIES(ALFA_Digitization) 
{
  DECLARE_ALGORITHM(ALFA_DigiAlg)
    DECLARE_TOOL(ALFA_PileUpTool)
}
