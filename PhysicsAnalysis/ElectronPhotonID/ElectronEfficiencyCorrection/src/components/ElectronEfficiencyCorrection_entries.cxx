#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"

DECLARE_TOOL_FACTORY( AsgElectronEfficiencyCorrectionTool )


DECLARE_FACTORY_ENTRIES( ElectronEfficiencyCorrection ) 
{
  DECLARE_ALGTOOL( AsgElectronEfficiencyCorrectionTool )
}


