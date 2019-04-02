#include <GaudiKernel/DeclareFactoryEntries.h>
#include <MuonTruthOverlay/CscSimDataOverlay.h>
#include <MuonTruthOverlay/MuonSimDataOverlay.h>

DECLARE_ALGORITHM_FACTORY( CscSimDataOverlay )
DECLARE_ALGORITHM_FACTORY( MuonSimDataOverlay )

DECLARE_FACTORY_ENTRIES( MuonTruthOverlay ) {
  DECLARE_ALGORITHM( CscSimDataOverlay )
  DECLARE_ALGORITHM( MuonSimDataOverlay )
}
