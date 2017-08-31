#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkMuons/dimuonTaggingTool.h"
#include "DerivationFrameworkMuons/isolationDecorator.h"
#include "DerivationFrameworkMuons/mcpDecorator.h"
#include "DerivationFrameworkMuons/MuonTPExtrapolationTool.h"
#include "DerivationFrameworkMuons/IDTrackCaloDepositsDecoratorTool.h"
using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( dimuonTaggingTool )
DECLARE_TOOL_FACTORY( isolationDecorator )
DECLARE_TOOL_FACTORY( mcpDecorator )
DECLARE_TOOL_FACTORY (MuonTPExtrapolationTool)
DECLARE_TOOL_FACTORY (IDTrackCaloDepositsDecoratorTool)

DECLARE_FACTORY_ENTRIES( DerivationFrameworkMuons ) {
   DECLARE_TOOL( dimuonTaggingTool )
   DECLARE_TOOL( isolationDecorator )
   DECLARE_TOOL( mcpDecorator )
   DECLARE_TOOL(MuonTPExtrapolationTool)
   DECLARE_TOOL(IDTrackCaloDepositsDecoratorTool)
}

