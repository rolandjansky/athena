#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetV0Finder/InDetV0FinderTool.h"
#include "InDetV0Finder/InDetV0Finder.h"
//#include "InDetV0Finder/InDetSimpleV0Finder.h"

using namespace InDet;

DECLARE_TOOL_FACTORY( InDetV0FinderTool )

DECLARE_FACTORY_ENTRIES( InDetV0FinderTool )
{
    DECLARE_TOOL( InDetV0FinderTool )
}

DECLARE_ALGORITHM_FACTORY( InDetV0Finder )
//DECLARE_ALGORITHM_FACTORY( InDetSimpleV0Finder )

DECLARE_FACTORY_ENTRIES( InDetV0Finder )
{
    DECLARE_ALGORITHM( InDetV0Finder );
}

//DECLARE_FACTORY_ENTRIES( InDetSimpleV0Finder )
//{
//    DECLARE_ALGORITHM( InDetSimpleV0Finder );
//}
