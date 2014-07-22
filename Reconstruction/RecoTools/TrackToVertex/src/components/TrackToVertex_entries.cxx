#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrackToVertex/TrackToVertex.h"
#include "TrackToVertex/TrackToVertexAssociator.h"

using namespace Reco;

DECLARE_TOOL_FACTORY( TrackToVertex )
DECLARE_TOOL_FACTORY( TrackToVertexAssociator )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrackToVertex )
{
    DECLARE_TOOL( TrackToVertex )
    DECLARE_TOOL( TrackToVertexAssociator )
}


