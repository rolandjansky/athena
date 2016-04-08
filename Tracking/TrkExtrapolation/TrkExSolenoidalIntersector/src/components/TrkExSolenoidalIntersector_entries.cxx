#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkExSolenoidalIntersector/SolenoidalIntersector.h"

typedef Trk::SolenoidalIntersector TrkSolenoidalIntersector;
DECLARE_TOOL_FACTORY( TrkSolenoidalIntersector )

DECLARE_FACTORY_ENTRIES( TrkExSolenoidalIntersector )
{
    DECLARE_NAMESPACE_TOOL( Trk, SolenoidalIntersector )
}
