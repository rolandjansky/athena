#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkExStraightLineIntersector/StraightLineIntersector.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, StraightLineIntersector )
DECLARE_FACTORY_ENTRIES( TrkExStraightLineIntersector )
{
    DECLARE_NAMESPACE_TOOL( Trk, StraightLineIntersector )
}
