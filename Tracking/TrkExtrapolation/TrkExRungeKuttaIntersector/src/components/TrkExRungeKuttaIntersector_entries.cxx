#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkExRungeKuttaIntersector/IntersectorWrapper.h"
#include "TrkExRungeKuttaIntersector/RungeKuttaIntersector.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, IntersectorWrapper )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, RungeKuttaIntersector )
DECLARE_FACTORY_ENTRIES( TrkExRungeKuttaIntersector )
{
    DECLARE_NAMESPACE_TOOL( Trk, IntersectorWrapper )
    DECLARE_NAMESPACE_TOOL( Trk, RungeKuttaIntersector )
}
