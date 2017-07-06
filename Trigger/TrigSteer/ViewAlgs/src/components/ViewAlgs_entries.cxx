
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../MenuAlgView.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthViews,MenuAlgView )

DECLARE_FACTORY_ENTRIES( ViewAlgs )
{

  DECLARE_NAMESPACE_ALGORITHM( AthViews, MenuAlgView )
}

