
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../DumpDecisions.h"
#include "../PrescaleDecision.h"
#include "../MenuAlg.h"
#include "../MenuAlgView.h"
#include "../CopyPassing.h"


DECLARE_ALGORITHM_FACTORY( DumpDecisions )
DECLARE_ALGORITHM_FACTORY( PrescaleDecision )
DECLARE_ALGORITHM_FACTORY( MenuAlg )
DECLARE_ALGORITHM_FACTORY( CopyPassing )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews,MenuAlgView )

DECLARE_FACTORY_ENTRIES( ViewAlgs )
{
  DECLARE_ALGORITHM( DumpDecisions )
  DECLARE_ALGORITHM( PrescaleDecision )
  DECLARE_ALGORITHM( MenuAlg )
  DECLARE_ALGORITHM( CopyPassing )
  DECLARE_NAMESPACE_ALGORITHM( AthViews, MenuAlgView )
}

