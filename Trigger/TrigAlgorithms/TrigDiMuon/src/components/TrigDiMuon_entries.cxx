#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigDiMuon/TrigDiMuonFast.h"
#include "TrigDiMuon/TrigDiMuonExtrapolator.h"

DECLARE_ALGORITHM_FACTORY( TrigDiMuonFast )

DECLARE_TOOL_FACTORY( TrigDiMuonExtrapolator )

DECLARE_FACTORY_ENTRIES(TrigDiMuon) {
  DECLARE_ALGORITHM( TrigDiMuonFast )

  DECLARE_TOOL( TrigDiMuonExtrapolator )
}
