#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCalibAlgs/MuonCalibAlg.h"

using namespace MuonCalib;

DECLARE_ALGORITHM_FACTORY ( MuonCalibAlg )

DECLARE_FACTORY_ENTRIES(MuonCalibAlgs) {
  DECLARE_ALGORITHM(MuonCalibAlg)
}
