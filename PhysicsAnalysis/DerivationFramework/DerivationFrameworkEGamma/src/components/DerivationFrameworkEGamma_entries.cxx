#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkEGamma/ElectronLikelihoodPIDTool.h"
#include "DerivationFrameworkEGamma/ElectronsDirectionTool.h"
#include "DerivationFrameworkEGamma/ElectronMultiLeptonTool.h"
#include "DerivationFrameworkEGamma/EGammaPassSelectionWrapper.h"
using namespace DerivationFramework;
 
DECLARE_TOOL_FACTORY( ElectronLikelihoodPIDTool )
DECLARE_TOOL_FACTORY( ElectronsDirectionTool )
DECLARE_TOOL_FACTORY( ElectronMultiLeptonTool )
DECLARE_TOOL_FACTORY( EGammaPassSelectionWrapper )
 
DECLARE_FACTORY_ENTRIES( DerivationFrameworkEGamma ) {
   DECLARE_TOOL( ElectronLikelihoodPIDTool )
   DECLARE_TOOL( ElectronsDirectionTool )
   DECLARE_TOOL( ElectronMultiLeptonTool )
   DECLARE_TOOL( EGammaPassSelectionWrapper ) 
}
 
