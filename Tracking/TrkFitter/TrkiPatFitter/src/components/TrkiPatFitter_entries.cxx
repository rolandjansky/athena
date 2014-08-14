#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkiPatFitter/iPatGlobalFitter.h"
#include "TrkiPatFitter/MaterialAllocator.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, iPatFitter )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, iPatGlobalFitter )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MaterialAllocator )

DECLARE_FACTORY_ENTRIES( TrkiPatFitter )
{
    DECLARE_NAMESPACE_TOOL( Trk, iPatFitter )
    DECLARE_NAMESPACE_TOOL( Trk, iPatGlobalFitter )	
    DECLARE_NAMESPACE_TOOL( Trk, MaterialAllocator )	
}
