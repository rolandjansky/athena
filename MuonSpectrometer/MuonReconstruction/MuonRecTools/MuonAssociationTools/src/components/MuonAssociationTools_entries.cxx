#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonAssociationTools/MuonPatternSegmentAssociationTool.h"
#include "../MuonPRD_Provider.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonPatternSegmentAssociationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonPRD_Provider )

DECLARE_FACTORY_ENTRIES( MuonAssociationTools )
{
  DECLARE_NAMESPACE_TOOL( Muon, MuonPatternSegmentAssociationTool )
  DECLARE_NAMESPACE_TOOL( Muon, MuonPRD_Provider )
}

