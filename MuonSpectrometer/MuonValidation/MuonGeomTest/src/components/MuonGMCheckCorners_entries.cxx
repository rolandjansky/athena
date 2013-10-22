#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonGMCheckCorners.h"
#include "../MuonGMNtupleWriter.h"

DECLARE_ALGORITHM_FACTORY(MuonGMCheckCorners)
DECLARE_ALGORITHM_FACTORY(MuonGMNtupleWriter)
DECLARE_FACTORY_ENTRIES(MuonGeomTest) {
  DECLARE_ALGORITHM( MuonGMCheckCorners );
  DECLARE_ALGORITHM( MuonGMNtupleWriter );
}
