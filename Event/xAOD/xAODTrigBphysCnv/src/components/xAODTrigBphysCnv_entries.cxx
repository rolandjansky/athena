#include "../TrigL2BphysCnvTool.h"
#include "../TrigEFBphysCnvTool.h"
#include "../TrigL2BphysContainerCnvTool.h"
#include "../TrigEFBphysContainerCnvTool.h"

#include "../TrigL2BphysCnvAlg.h"
#include "../TrigEFBphysCnvAlg.h"
#include "../TrigxAODBphysReaderAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigL2BphysCnvTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker,  TrigL2BphysContainerCnvTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigL2BphysCnvAlg)

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker,  TrigEFBphysCnvTool)
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker,  TrigEFBphysContainerCnvTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigEFBphysCnvAlg)

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigxAODBphysReaderAlg)

