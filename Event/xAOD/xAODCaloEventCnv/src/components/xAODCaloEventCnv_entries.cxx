// $Id: xAODCaloEventCnv_entries.cxx 596346 2014-05-10 13:47:20Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../ClusterCreator.h"
#include "../ClusterDumper.h"
#include "../CaloClusterCnvTool.h"

DECLARE_ALGORITHM_FACTORY( ClusterCreator )
DECLARE_ALGORITHM_FACTORY( ClusterDumper )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, CaloClusterCnvTool )

DECLARE_FACTORY_ENTRIES( AODCaloCnvs ) {
   DECLARE_ALGORITHM( ClusterCreator )
   DECLARE_ALGORITHM( ClusterDumper )
   DECLARE_NAMESPACE_TOOL( xAODMaker, CaloClusterCnvTool )
}
