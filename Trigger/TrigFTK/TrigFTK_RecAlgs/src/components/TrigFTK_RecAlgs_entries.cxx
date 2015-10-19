#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigFTK_RecAlgs/TrigFTK_VxPrimary.h"
#include "TrigFTK_RecAlgs/TrigFTK_VxPrimaryAllTE.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TrigFTK_VxPrimary )
DECLARE_ALGORITHM_FACTORY( TrigFTK_VxPrimaryAllTE )

DECLARE_FACTORY_ENTRIES( TrigFTK_RecAlgs )
{
    DECLARE_ALGORITHM( TrigFTK_VxPrimary )
    DECLARE_ALGORITHM( TrigFTK_VxPrimaryAllTE )
}

