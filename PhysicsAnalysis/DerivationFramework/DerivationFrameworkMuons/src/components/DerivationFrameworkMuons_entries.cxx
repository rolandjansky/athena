#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkMuons/dimuonTaggingTool.h"
#include "DerivationFrameworkMuons/isolationDecorator.h"
<<<<<<< HEAD
//#include "DerivationFrameworkMuons/IsolationAugmentationToolMuons.h"
=======
#include "DerivationFrameworkMuons/mcpDecorator.h"
#include "DerivationFrameworkMuons/MuonTPExtrapolationTool.h"
#include "DerivationFrameworkMuons/IDTrackCaloDepositsDecoratorTool.h"
>>>>>>> Moved muonTP files into derivation framework
using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( dimuonTaggingTool )
DECLARE_TOOL_FACTORY( isolationDecorator )
<<<<<<< HEAD
//DECLARE_TOOL_FACTORY( IsolationAugmentationToolMuons )
=======
DECLARE_TOOL_FACTORY( mcpDecorator )
DECLARE_TOOL_FACTORY (MuonTPExtrapolationTool)
DECLARE_TOOL_FACTORY (IDTrackCaloDepositsDecoratorTool)
>>>>>>> Moved muonTP files into derivation framework

DECLARE_FACTORY_ENTRIES( DerivationFrameworkMuons ) {
   DECLARE_TOOL( dimuonTaggingTool )
   DECLARE_TOOL( isolationDecorator )
<<<<<<< HEAD
   //DECLARE_TOOL( IsolationAugmentationToolMuons )
=======
   DECLARE_TOOL( mcpDecorator )
   DECLARE_TOOL(MuonTPExtrapolationTool)
   DECLARE_TOOL(IDTrackCaloDepositsDecoratorTool)
>>>>>>> Moved muonTP files into derivation framework
}

