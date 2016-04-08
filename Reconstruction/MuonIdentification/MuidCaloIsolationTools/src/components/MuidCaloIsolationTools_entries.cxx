#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuidCaloIsolationTools/MuidTrackIsolation.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidTrackIsolation )
DECLARE_FACTORY_ENTRIES( MuidCaloIsolationTools )
{
    DECLARE_NAMESPACE_TOOL( Rec, MuidTrackIsolation )
}

