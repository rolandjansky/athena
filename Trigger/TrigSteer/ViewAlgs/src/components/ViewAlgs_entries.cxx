
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../DumpDecisions.h"
#include "../PrescaleDecision.h"
#include "../MenuAlg.h"


DECLARE_ALGORITHM_FACTORY( DumpDecisions )
DECLARE_ALGORITHM_FACTORY( PrescaleDecision )
DECLARE_ALGORITHM_FACTORY( MenuAlg )

DECLARE_FACTORY_ENTRIES( ViewAlgs )
{
  DECLARE_ALGORITHM( DumpDecisions )
  DECLARE_ALGORITHM( PrescaleDecision )
  DECLARE_ALGORITHM( MenuAlg )
}
