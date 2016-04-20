#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkEGamma/PhotonsDirectionTool.h"
#include "DerivationFrameworkEGamma/EGammaPassSelectionWrapper.h"
#include "DerivationFrameworkEGamma/EGInvariantMassTool.h"
#include "DerivationFrameworkEGamma/EGTransverseMassTool.h"
#include "DerivationFrameworkEGamma/EGSelectionToolWrapper.h"
#include "DerivationFrameworkEGamma/BkgElectronClassification.h"


using namespace DerivationFramework; 
DECLARE_TOOL_FACTORY( PhotonsDirectionTool )
DECLARE_TOOL_FACTORY( EGammaPassSelectionWrapper )
DECLARE_TOOL_FACTORY( EGInvariantMassTool )
DECLARE_TOOL_FACTORY( EGTransverseMassTool )
DECLARE_TOOL_FACTORY( EGSelectionToolWrapper )
DECLARE_TOOL_FACTORY( BkgElectronClassification )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkEGamma ) {
   DECLARE_TOOL( PhotonsDirectionTool )
   DECLARE_TOOL( EGammaPassSelectionWrapper ) 
   DECLARE_TOOL( EGInvariantMassTool )
   DECLARE_TOOL( EGTransverseMassMassTool )
   DECLARE_TOOL( EGSelectionToolWrapper )
   DECLARE_TOOL( BkgElectronClassification )
}
 
