// $Id: xAODTrigCaloCnv_entries.cxx 592334 2014-04-10 11:01:56Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../TrigCaloClusterCnvTool.h"
#include "../TrigCaloClusterCnvAlg.h"
#include "../TrigEMClusterCnvTool.h"
#include "../TrigEMClusterCnvAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigCaloClusterCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigCaloClusterCnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigEMClusterCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigEMClusterCnvAlg )

DECLARE_FACTORY_ENTRIES( xAODTrigCaloCnv ) {

   DECLARE_NAMESPACE_TOOL( xAODMaker, TrigCaloClusterCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigCaloClusterCnvAlg )
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrigEMClusterCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigEMClusterCnvAlg )

}
