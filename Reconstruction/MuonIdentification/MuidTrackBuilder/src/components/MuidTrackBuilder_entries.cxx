#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuidTrackBuilder/CombinedMuonTrackBuilder.h"
#include "MuidTrackBuilder/MuidMuonRecovery.h"
#include "MuidTrackBuilder/MuonMatchQuality.h"
#include "MuidTrackBuilder/MuonTrackQuery.h"
#include "MuidTrackBuilder/OutwardsCombinedMuonTrackBuilder.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rec, CombinedMuonTrackBuilder )
DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidMuonRecovery )
DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuonMatchQuality )
DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuonTrackQuery )
DECLARE_NAMESPACE_TOOL_FACTORY( Rec, OutwardsCombinedMuonTrackBuilder )

DECLARE_FACTORY_ENTRIES( MuidTrackBuilder )
{
  DECLARE_NAMESPACE_TOOL( Rec, CombinedMuonTrackBuilder )
  DECLARE_NAMESPACE_TOOL( Rec, MuidMuonRecovery )
  DECLARE_NAMESPACE_TOOL( Rec, MuonMatchQuality )
  DECLARE_NAMESPACE_TOOL( Rec, MuonTrackQuery )
  DECLARE_NAMESPACE_TOOL( Rec, OutwardsCombinedMuonTrackBuilder )    
}
