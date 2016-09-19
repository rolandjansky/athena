#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../EMBremCollectionBuilder.h"
#include "../EMClusterTool.h"
#include "../EMConversionBuilder.h"
#include "../EMFourMomBuilder.h"
#include "../EMShowerBuilder.h"
#include "../EMTrackMatchBuilder.h"
#include "../EMVertexBuilder.h"
#include "../FourMomCombiner.h"
#include "../egammaCheckEnergyDepositTool.h"
#include "../egammaOQFlagsBuilder.h"
#include "../EMPIDBuilder.h"
#include "../egammaSwTool.h"
#include "../electronSuperClusterBuilder.h"
#include "../photonSuperClusterBuilder.h"
#include "../egammaTopoClusterCopier.h"

DECLARE_TOOL_FACTORY     ( EMBremCollectionBuilder      )
DECLARE_TOOL_FACTORY     ( EMClusterTool                )
DECLARE_TOOL_FACTORY     ( EMConversionBuilder          )
DECLARE_TOOL_FACTORY     ( EMFourMomBuilder             )
DECLARE_TOOL_FACTORY     ( EMShowerBuilder              )
DECLARE_TOOL_FACTORY     ( EMTrackMatchBuilder          )
DECLARE_TOOL_FACTORY     ( EMVertexBuilder              )
DECLARE_TOOL_FACTORY     ( FourMomCombiner              )
DECLARE_TOOL_FACTORY     ( egammaCheckEnergyDepositTool )
DECLARE_TOOL_FACTORY     ( egammaOQFlagsBuilder         )
DECLARE_TOOL_FACTORY     ( EMPIDBuilder                 )
DECLARE_TOOL_FACTORY     ( egammaSwTool                 )
DECLARE_TOOL_FACTORY     ( electronSuperClusterBuilder  )
DECLARE_TOOL_FACTORY     ( photonSuperClusterBuilder    )
DECLARE_TOOL_FACTORY     ( egammaTopoClusterCopier      )

DECLARE_FACTORY_ENTRIES(egammaTools) {
    DECLARE_TOOL     ( EMBremCollectionBuilder      )
    DECLARE_TOOL     ( EMClusterTool                )
    DECLARE_TOOL     ( EMConversionBuilder          )
    DECLARE_TOOL     ( EMFourMomBuilder             )
    DECLARE_TOOL     ( EMShowerBuilder              )
    DECLARE_TOOL     ( EMTrackMatchBuilder          )
    DECLARE_TOOL     ( EMVertexBuilder              )
    DECLARE_TOOL     ( FourMomCombiner              )
    DECLARE_TOOL     ( egammaCheckEnergyDepositTool )
    DECLARE_TOOL     ( egammaOQFlagsBuilder         )
    DECLARE_TOOL     ( EMPIDBuilder                 )
    DECLARE_TOOL     ( egammaSwTool                 )
    DECLARE_TOOL     ( electronSuperClusterBuilder  )
    DECLARE_TOOL     ( photonSuperClusterBuilder    )
    DECLARE_TOOL     ( egammaTopoClusterCopier      )
}

