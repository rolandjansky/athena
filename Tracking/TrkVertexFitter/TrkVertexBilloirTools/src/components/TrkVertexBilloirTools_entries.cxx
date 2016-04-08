#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkVertexBilloirTools/FastVertexFitter.h"
#include "TrkVertexBilloirTools/FullVertexFitter.h"

using namespace Trk ;

DECLARE_TOOL_FACTORY( FastVertexFitter )
DECLARE_TOOL_FACTORY( FullVertexFitter )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkVertexBilloirTools )
{
    DECLARE_TOOL( FastVertexFitter )
    DECLARE_TOOL( FullVertexFitter )
}
