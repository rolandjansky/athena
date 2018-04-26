// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "PhotonEfficiencyCorrection/AsgPhotonEfficiencyCorrectionTool.h"

#include "../testAthenaPhotonAlg.h"

DECLARE_TOOL_FACTORY( AsgPhotonEfficiencyCorrectionTool )

DECLARE_ALGORITHM_FACTORY( testAthenaPhotonAlg )

DECLARE_FACTORY_ENTRIES( PhotonEfficiencyCorrection ) 
{
  DECLARE_TOOL( AsgPhotonEfficiencyCorrectionTool )
  DECLARE_ALGORITHM( testAthenaPhotonAlg )  

}