// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <MuonAnalysisAlgorithms/MuonCalibrationAndSmearingAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::MuonCalibrationAndSmearingAlg)

DECLARE_FACTORY_ENTRIES(MuonAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::MuonCalibrationAndSmearingAlg)
}
