#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigL2CosmicMuonHypo/CosmicMuonHypo.h"
#include "TrigL2CosmicMuonHypo/CosmicTrtHypo.h"
#include "TrigL2CosmicMuonHypo/CombinedIDMuonHypo.h"
#include "TrigL2CosmicMuonHypo/CombinedIDHalfTracksHypo.h"
DECLARE_ALGORITHM_FACTORY(CosmicMuonHypo)
DECLARE_ALGORITHM_FACTORY(CosmicTrtHypo)
DECLARE_ALGORITHM_FACTORY(CombinedIDMuonHypo)
DECLARE_ALGORITHM_FACTORY(CombinedIDHalfTracksHypo)
DECLARE_FACTORY_ENTRIES(TrigL2CosmicMuonHypo) {
  DECLARE_ALGORITHM(CosmicMuonHypo);
  DECLARE_ALGORITHM(CosmicTrtHypo);
  DECLARE_ALGORITHM(CombinedIDMuonHypo);
  DECLARE_ALGORITHM(CombinedIDHalfTracksHypo);
}
