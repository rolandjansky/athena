#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronMultiLeptonSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgForwardElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/EGammaAmbiguityTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronChargeIDSelectorTool.h"
#include "ElectronPhotonSelectorTools/AsgDeadHVCellRemovalTool.h"

DECLARE_TOOL_FACTORY( AsgElectronIsEMSelector )
DECLARE_TOOL_FACTORY( AsgElectronMultiLeptonSelector )
DECLARE_TOOL_FACTORY( AsgElectronLikelihoodTool )
DECLARE_TOOL_FACTORY( AsgPhotonIsEMSelector )
DECLARE_TOOL_FACTORY( AsgForwardElectronIsEMSelector )
DECLARE_TOOL_FACTORY( EGammaAmbiguityTool )
DECLARE_TOOL_FACTORY( AsgElectronChargeIDSelectorTool )
DECLARE_TOOL_FACTORY( AsgDeadHVCellRemovalTool )
DECLARE_FACTORY_ENTRIES( ElectronPhotonSelectorTools ) 
{
  DECLARE_ALGTOOL( AsgElectronIsEMSelector )
  DECLARE_ALGTOOL( AsgElectronMultiLeptonSelector )
  DECLARE_ALGTOOL( AsgElectronLikelihoodTool )
  DECLARE_ALGTOOL( AsgPhotonIsEMSelector )
  DECLARE_ALGTOOL( AsgForwardElectronIsEMSelector )
  DECLARE_ALGTOOL( EGammaAmbiguityTool )
  DECLARE_ALGTOOL( AsgDeadHVCellRemovalTool )
}


