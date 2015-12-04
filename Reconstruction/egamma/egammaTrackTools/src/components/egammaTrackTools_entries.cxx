#include "../egammaTrkRefitterTool.h"
#include "../EMExtrapolationTools.h"
#include "../CaloCluster_OnTrackBuilder.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY     ( egammaTrkRefitterTool    )
DECLARE_TOOL_FACTORY     ( EMExtrapolationTools     )
DECLARE_TOOL_FACTORY     (CaloCluster_OnTrackBuilder)

DECLARE_FACTORY_ENTRIES(egammaTrackTools) {
    DECLARE_TOOL     ( egammaTrkRefitterTool    )
    DECLARE_TOOL     ( EMExtrapolationTools     )
    DECLARE_TOOL     ( CaloCluster_OnTrackBuilder)
}
