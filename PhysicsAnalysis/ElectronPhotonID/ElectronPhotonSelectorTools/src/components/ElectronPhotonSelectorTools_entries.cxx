#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronMultiLeptonSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"

DECLARE_TOOL_FACTORY( AsgElectronIsEMSelector )
DECLARE_TOOL_FACTORY( AsgElectronMultiLeptonSelector )
DECLARE_TOOL_FACTORY( AsgElectronLikelihoodTool )
DECLARE_TOOL_FACTORY( AsgPhotonIsEMSelector )


DECLARE_FACTORY_ENTRIES( ElectronPhotonSelectorTools ) 
{
  DECLARE_ALGTOOL( AsgElectronIsEMSelector )
  DECLARE_ALGTOOL( AsgElectronMultiLeptonSelector )
  DECLARE_ALGTOOL( AsgElectronLikelihoodTool )
  DECLARE_ALGTOOL( AsgPhotonIsEMSelector )
}


