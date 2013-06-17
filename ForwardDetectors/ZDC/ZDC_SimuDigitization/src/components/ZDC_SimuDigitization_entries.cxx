#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ZDC_SimuDigitization/ZDC_DigiTop.h"
#include "ZDC_SimuDigitization/ZDC_PileUpTool.h"

DECLARE_ALGORITHM_FACTORY(ZDC_DigiTop)

DECLARE_TOOL_FACTORY( ZDC_PileUpTool )

DECLARE_FACTORY_ENTRIES  (ZDC_SimuDigitization) 
{
  DECLARE_ALGORITHM(ZDC_DigiTop)
    DECLARE_TOOL( ZDC_PileUpTool )
}

