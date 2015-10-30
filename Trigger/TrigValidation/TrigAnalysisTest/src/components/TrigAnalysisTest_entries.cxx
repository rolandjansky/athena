/** J.Montejo 15/10/2015 */

#include "GaudiKernel/DeclareFactoryEntries.h"

//derived algorithm classes
#include "TrigAnalysisTest/TriggersFiredTest.h"

DECLARE_ALGORITHM_FACTORY( TriggersFiredTest )

DECLARE_FACTORY_ENTRIES( TrigAnalysisTest ) {
  DECLARE_ALGORITHM( TriggersFiredTest );
}
