#include <GaudiKernel/DeclareFactoryEntries.h>
#include <MuonTruthOverlay/MuonSimDataOverlay.h>

DECLARE_ALGORITHM_FACTORY( MuonSimDataOverlay )

DECLARE_FACTORY_ENTRIES( MuonTruthOverlay ) {
  DECLARE_ALGORITHM( MuonSimDataOverlay )
}
