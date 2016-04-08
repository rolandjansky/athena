#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkMuons/dimuonTaggingTool.h"
#include "DerivationFrameworkMuons/isolationDecorator.h"
//#include "DerivationFrameworkMuons/IsolationAugmentationToolMuons.h"
using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( dimuonTaggingTool )
DECLARE_TOOL_FACTORY( isolationDecorator )
//DECLARE_TOOL_FACTORY( IsolationAugmentationToolMuons )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkMuons ) {
   DECLARE_TOOL( dimuonTaggingTool )
   DECLARE_TOOL( isolationDecorator )
   //DECLARE_TOOL( IsolationAugmentationToolMuons )
}

