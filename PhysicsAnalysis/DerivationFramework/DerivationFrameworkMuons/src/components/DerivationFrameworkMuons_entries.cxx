#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkMuons/MCPInDetRecommendations.h"
#include "DerivationFrameworkMuons/MCPHighPtRecommendations.h"
using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( MCPInDetRecommendations )
DECLARE_TOOL_FACTORY( MCPHighPtRecommendations )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkMuons ) {
   DECLARE_TOOL( MCPInDetRecommendations )
   DECLARE_TOOL( MCPHighPtRecommendations )
}

