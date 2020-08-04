#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkMuons/dimuonTaggingTool.h"
#include "DerivationFrameworkMuons/isolationDecorator.h"
#include "DerivationFrameworkMuons/mcpDecorator.h"
#include "DerivationFrameworkMuons/MuonTPExtrapolationTool.h"
#include "DerivationFrameworkMuons/IDTrackCaloDepositsDecoratorTool.h"
#include "DerivationFrameworkMuons/MuonIDCovMatrixDecorator.h"
#include "DerivationFrameworkMuons/MuonTruthClassifierFallback.h"
#include "DerivationFrameworkMuons/MuonTruthIsolationTool.h"
#include "DerivationFrameworkMuons/MuonJetDrTool.h"
#include "DerivationFrameworkMuons/MuonTPJpsiVertexFittingAlg.h"
#include "DerivationFrameworkMuons/TrackAssociatedCaloSampleDecorator.h"


#include "DerivationFrameworkMuons/VertexDecoratorAlg.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( dimuonTaggingTool )
DECLARE_TOOL_FACTORY( isolationDecorator )
DECLARE_TOOL_FACTORY( mcpDecorator )
DECLARE_TOOL_FACTORY (MuonTPExtrapolationTool)
DECLARE_TOOL_FACTORY (IDTrackCaloDepositsDecoratorTool)
DECLARE_TOOL_FACTORY (MuonIDCovMatrixDecorator)
DECLARE_TOOL_FACTORY (MuonTruthClassifierFallback)
DECLARE_TOOL_FACTORY (MuonTruthIsolationTool)
DECLARE_TOOL_FACTORY (MuonJetDrTool)
DECLARE_TOOL_FACTORY (TrackAssociatedCaloSampleDecorator) 
DECLARE_ALGORITHM_FACTORY(VertexDecoratorAlg)
DECLARE_ALGORITHM_FACTORY(MuonTPJpsiVertexFittingAlg)


DECLARE_FACTORY_ENTRIES( DerivationFrameworkMuons ) {
   DECLARE_TOOL( dimuonTaggingTool )
   DECLARE_TOOL( isolationDecorator )
   DECLARE_TOOL( mcpDecorator )
   DECLARE_TOOL(MuonTPExtrapolationTool)
   DECLARE_TOOL(IDTrackCaloDepositsDecoratorTool)
   DECLARE_TOOL(MuonIDCovMatrixDecorator)
   DECLARE_TOOL(MuonTruthClassifierFallback)
   DECLARE_TOOL(MuonTruthIsolationTool)
   DECLARE_TOOL(MuonJetDrTool)
   DECLARE_TOOL(TrackAssociatedCaloSampleDecorator)
   
   DECLARE_ALGORITHM(MuonTPJpsiVertexFittingAlg)
   DECLARE_ALGORITHM(VertexDecoratorAlg)
}

