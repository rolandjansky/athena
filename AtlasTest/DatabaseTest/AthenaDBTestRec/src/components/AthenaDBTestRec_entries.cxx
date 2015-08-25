#include "GaudiKernel/DeclareFactoryEntries.h"
#include "AthenaDBTestRec/TestCoolRecRead.h"
#include "AthenaDBTestRec/DetStoreDump.h"

DECLARE_ALGORITHM_FACTORY( TestCoolRecRead )
DECLARE_ALGORITHM_FACTORY( DetStoreDump )

DECLARE_FACTORY_ENTRIES( AthenaDBTestRec ) 
{
    DECLARE_ALGORITHM( TestCoolRecRead )
    DECLARE_ALGORITHM( DetStoreDump )
}

