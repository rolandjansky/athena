#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkTau/TauSelectionWrapper.h"
#include "DerivationFrameworkTau/TauTruthMatchingWrapper.h"
#include "DerivationFrameworkTau/TauPVRefitTool.h"
#include "DerivationFrameworkTau/TauPVTrkSelectionTool.h"
#include "DerivationFrameworkTau/TauOverlappingElectronLLHDecoratorWrapper.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( TauSelectionWrapper )
DECLARE_TOOL_FACTORY( TauTruthMatchingWrapper )
DECLARE_TOOL_FACTORY( TauPVRefitTool )
DECLARE_TOOL_FACTORY( TauPVTrkSelectionTool )
DECLARE_TOOL_FACTORY( TauOverlappingElectronLLHDecoratorWrapper )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkTau ) {
   DECLARE_TOOL( TauSelectionWrapper )
   DECLARE_TOOL( TauTruthMatchingWrapper )
   DECLARE_TOOL( TauPVRefitTool )
   DECLARE_TOOL( TauPVTrkSelectionTool )
   DECLARE_TOOL( TauOverlappingElectronLLHDecoratorWrapper )
}
