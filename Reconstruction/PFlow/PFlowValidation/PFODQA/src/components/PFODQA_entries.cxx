#include "GaudiKernel/DeclareFactoryEntries.h"
	
// Local include(s):
#include "../PhysValPFO.h"
#include "../PhysValCluster.h"
	
DECLARE_TOOL_FACTORY( PhysValPFO )
DECLARE_TOOL_FACTORY( PhysValCluster )
	
DECLARE_FACTORY_ENTRIES( PFODQA ) {
  DECLARE_ALGTOOL( PhysValPFO )
  DECLARE_ALGTOOL( PhysValCluster )
}
