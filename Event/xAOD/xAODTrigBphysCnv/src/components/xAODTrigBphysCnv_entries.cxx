// $Id:$
//
//Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
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

DECLARE_FACTORY_ENTRIES( xAODTrigBphysCnv ) {
    DECLARE_NAMESPACE_TOOL      ( xAODMaker, TrigL2BphysCnvTool )
    DECLARE_NAMESPACE_ALGORITHM ( xAODMaker, TrigL2BphysCnvAlg  )
    DECLARE_NAMESPACE_TOOL      ( xAODMaker, TrigEFBphysCnvTool )
    DECLARE_NAMESPACE_TOOL      ( xAODMaker, TrigEFBphysContainerCnvTool )
    DECLARE_NAMESPACE_TOOL      ( xAODMaker, TrigL2BphysContainerCnvTool )
    DECLARE_NAMESPACE_ALGORITHM ( xAODMaker, TrigEFBphysCnvAlg  )
    DECLARE_NAMESPACE_ALGORITHM ( xAODMaker, TrigxAODBphysReaderAlg )
}







