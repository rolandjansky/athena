/** J.Montejo 15/10/2015 */

#include "GaudiKernel/DeclareFactoryEntries.h"

//derived algorithm classes
#include "TrigAnalysisTest/AthenaTestHarness.h"

DECLARE_ALGORITHM_FACTORY( AthenaTestHarness )

DECLARE_FACTORY_ENTRIES( TrigAnalysisTest ) {
  DECLARE_ALGORITHM( AthenaTestHarness );
}
