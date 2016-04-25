/** R.Goncalo - 21/10/2007 - to add tests for TrigDecisionTool */

#include "GaudiKernel/DeclareFactoryEntries.h"

// derived algorithm classes
#include "TrigValAlgs/TrigCountDumper.h"
#include "TrigValAlgs/TrigDecisionChecker.h"
#include "TrigValAlgs/TrigEDMChecker.h"
#include "TrigValAlgs/TrigEDMAuxChecker.h"
#include "TrigValAlgs/TrigProblemFinder.h"
#include "TrigValAlgs/TrigSlimValAlg.h"

using namespace TrigDec;

DECLARE_ALGORITHM_FACTORY( TrigCountDumper )
DECLARE_ALGORITHM_FACTORY( TrigDecisionChecker )
DECLARE_ALGORITHM_FACTORY( TrigEDMChecker )
DECLARE_ALGORITHM_FACTORY( TrigEDMAuxChecker )
DECLARE_ALGORITHM_FACTORY( TrigProblemFinder )
DECLARE_ALGORITHM_FACTORY( TrigSlimValAlg )

DECLARE_FACTORY_ENTRIES( TrigAnalysisTest ) {
  DECLARE_ALGORITHM( TrigCountDumper )
  DECLARE_ALGORITHM( TrigDecisionChecker )
  DECLARE_ALGORITHM( TrigEDMChecker )
  DECLARE_ALGORITHM( TrigEDMAuxChecker )
  DECLARE_ALGORITHM( TrigProblemFinder )
  DECLARE_ALGORITHM( TrigSlimValAlg )
}

