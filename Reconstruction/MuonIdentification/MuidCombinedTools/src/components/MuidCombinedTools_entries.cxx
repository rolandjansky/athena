#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuidCombinedTools/MuidBackTracker.h"
#include "MuidCombinedTools/MuidVertexCombiner.h"
#include "MuidCombinedTools/MuidMatchMaker.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidBackTracker )
DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidMatchMaker )
DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidVertexCombiner )

DECLARE_FACTORY_ENTRIES( MuidCombinedTools )
{
    DECLARE_NAMESPACE_TOOL( Rec, MuidBackTracker )
    DECLARE_NAMESPACE_TOOL( Rec, MuidVertexCombiner )
    DECLARE_NAMESPACE_TOOL( Rec, MuidMatchMaker )	
	 
}

