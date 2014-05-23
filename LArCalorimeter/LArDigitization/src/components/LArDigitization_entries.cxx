//
//  Entry file for ATHENA component libraries
//

#include "LArDigitization/LArDigitMaker.h"
#include "LArDigitization/LArHitMerger.h"
#include "LArDigitization/LArHitFilter.h"
#include "LArDigitization/LArPileUpTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( LArDigitMaker )
DECLARE_ALGORITHM_FACTORY( LArHitMerger )
DECLARE_ALGORITHM_FACTORY( LArHitFilter )
DECLARE_TOOL_FACTORY( LArPileUpTool )

DECLARE_FACTORY_ENTRIES(LArDigitization) {

  DECLARE_ALGORITHM(LArDigitMaker)
  DECLARE_ALGORITHM(LArHitMerger)
  DECLARE_ALGORITHM(LArHitFilter)
  DECLARE_TOOL(LArPileUpTool)

}
