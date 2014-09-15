#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonTruthTrackBuilder.h"
#include "../MuonTruthSummaryTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonTruthTrackBuilder )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonTruthSummaryTool )

DECLARE_FACTORY_ENTRIES( MuonAssociationTools )
{
  DECLARE_NAMESPACE_TOOL( Muon, MuonTruthTrackBuilder )
  DECLARE_NAMESPACE_TOOL( Muon, MuonTruthSummaryTool )
}

