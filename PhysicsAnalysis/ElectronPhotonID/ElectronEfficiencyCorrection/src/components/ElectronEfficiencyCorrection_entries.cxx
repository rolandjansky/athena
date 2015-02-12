#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"

#include "../testAthenaElectronAlg.h"

DECLARE_TOOL_FACTORY( AsgElectronEfficiencyCorrectionTool )
DECLARE_ALGORITHM_FACTORY( testAthenaElectronAlg )

DECLARE_FACTORY_ENTRIES( ElectronEfficiencyCorrection ) 
{
  DECLARE_ALGTOOL( AsgElectronEfficiencyCorrectionTool )
  DECLARE_ALGTOOL( testAthenaElectronAlg )
}



