// $Id: xAODCaloEventCnv_entries.cxx 581807 2014-02-06 08:40:10Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../ClusterCreator.h"
#include "../ClusterDumper.h"


DECLARE_ALGORITHM_FACTORY( ClusterCreator )
DECLARE_ALGORITHM_FACTORY( ClusterDumper )

DECLARE_FACTORY_ENTRIES( AODCaloCnvs ) {
   DECLARE_ALGORITHM( ClusterCreator )
   DECLARE_ALGORITHM( ClusterDumper )

}
