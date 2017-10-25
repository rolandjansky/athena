#include "../EventInfoCnvTool.h"
#include "../EventInfoSelectorTool.h"
#include "../EventInfoCnvAlg.h"
#include "../EventInfoReaderAlg.h"
#include "../EventDuplicateFinderAlg.h"

#include "../EventInfoMCWeightFixAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EventInfoCnvTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EventInfoSelectorTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, EventInfoCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, EventInfoReaderAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, EventDuplicateFinderAlg )

DECLARE_ALGORITHM_FACTORY( EventInfoMCWeightFixAlg )

