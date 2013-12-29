#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetV0Finder/InDetV0Finder.h"
#include "InDetV0Finder/InDetSimpleV0Finder.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( InDetV0Finder )
DECLARE_ALGORITHM_FACTORY( InDetSimpleV0Finder )

DECLARE_FACTORY_ENTRIES( InDetV0Finder )
{
    DECLARE_ALGORITHM( InDetV0Finder );
}


DECLARE_FACTORY_ENTRIES( InDetSimpleV0Finder )
{
    DECLARE_ALGORITHM( InDetSimpleV0Finder );
}
