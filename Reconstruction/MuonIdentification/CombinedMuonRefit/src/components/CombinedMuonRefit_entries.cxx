#include "../ExtrapolateMuonToIP.h"
#include "../ExtrapolateMuonToIPTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( ExtrapolateMuonToIP )
DECLARE_TOOL_FACTORY( ExtrapolateMuonToIPTool )

DECLARE_FACTORY_ENTRIES(CombinedMuonRefit) {
    DECLARE_ALGORITHM( ExtrapolateMuonToIP )
    DECLARE_TOOL( ExtrapolateMuonToIPTool )
}
