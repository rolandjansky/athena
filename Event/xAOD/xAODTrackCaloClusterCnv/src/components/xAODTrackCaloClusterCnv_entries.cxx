// $Id: xAODTrackCaloClusterCnv_entries.cxx $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../TrackCaloClusterCompressorTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrackCaloClusterCompressorTool )

DECLARE_FACTORY_ENTRIES( xAODCreatorAlgs ) {
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrackCaloClusterCompressorTool )
}