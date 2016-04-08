#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkExEngine/ExtrapolationEngine.h"
#include "TrkExEngine/StaticEngine.h"
#include "TrkExEngine/StaticNavigationEngine.h"
#include "TrkExEngine/MaterialEffectsEngine.h"
#include "TrkExEngine/PropagationEngine.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( ExtrapolationEngine )
DECLARE_TOOL_FACTORY( MaterialEffectsEngine )
DECLARE_TOOL_FACTORY( StaticEngine )
DECLARE_TOOL_FACTORY( StaticNavigationEngine )
DECLARE_TOOL_FACTORY( PropagationEngine )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkExEngine )
{
    DECLARE_TOOL( ExtrapolationEngine )
    DECLARE_TOOL( MaterialEffectsEngine )        
    DECLARE_TOOL( StaticEngine )
    DECLARE_TOOL( StaticNavigationEngine )
    DECLARE_TOOL( PropagationEngine )
}


