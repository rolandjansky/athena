#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkV0Fitter/TrkV0VertexFitter.h"

using namespace Trk ;

DECLARE_TOOL_FACTORY( TrkV0VertexFitter )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkV0Fitter )
{
    DECLARE_TOOL( TrkV0VertexFitter )
}
