/** J.Montejo 15/10/2015 */

#include "GaudiKernel/DeclareFactoryEntries.h"

//derived algorithm classes
#include "../AthenaTestHarness.h"
#include "../TriggersFiredTest.h"

DECLARE_ALGORITHM_FACTORY( AthenaTestHarness )
DECLARE_ALGORITHM_FACTORY( TriggersFiredTest )

DECLARE_FACTORY_ENTRIES( TrigAnalysisTest ) {
  DECLARE_ALGORITHM( AthenaTestHarness )
  DECLARE_ALGORITHM( TriggersFiredTest )
}
