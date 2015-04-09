
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "TrigMuonMatching/TrigMuonMatching.h"
#include "../TrigMuonMatching_example.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigMuonMatching )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trig, TrigMuonMatching_example )

DECLARE_FACTORY_ENTRIES( TrigMuonMatching ) {

   DECLARE_NAMESPACE_TOOL( Trig, TrigMuonMatching )

   DECLARE_NAMESPACE_ALGORITHM( Trig, TrigMuonMatching_example )

}
