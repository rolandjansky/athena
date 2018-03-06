// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <MuonAnalysisAlgorithms/MuonCalibrationAndSmearingAlg.h>
#include <MuonAnalysisAlgorithms/MuonIsolationAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::MuonCalibrationAndSmearingAlg)
DECLARE_ALGORITHM_FACTORY (CP::MuonIsolationAlg)

DECLARE_FACTORY_ENTRIES(MuonAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::MuonCalibrationAndSmearingAlg)
  DECLARE_ALGORITHM (CP::MuonIsolationAlg)
}
