#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuGirl/MuGirl.h"
#include "../MuGirlRecoTool.h"

using namespace MuGirlNS;

DECLARE_ALGORITHM_FACTORY( MuGirl )
DECLARE_TOOL_FACTORY( MuGirlRecoTool )

DECLARE_FACTORY_ENTRIES( MuGirl )
{
    DECLARE_ALGORITHM( MuGirl )
    DECLARE_TOOL( MuGirlRecoTool )
}

