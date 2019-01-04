// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <MuonAnalysisAlgorithms/MuonCalibrationAndSmearingAlg.h>
#include <MuonAnalysisAlgorithms/MuonIsolationAlg.h>
#include <MuonAnalysisAlgorithms/MuonEfficiencyScaleFactorAlg.h>
#include <MuonAnalysisAlgorithms/MuonTriggerEfficiencyScaleFactorAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::MuonCalibrationAndSmearingAlg)
DECLARE_ALGORITHM_FACTORY (CP::MuonIsolationAlg)
DECLARE_ALGORITHM_FACTORY (CP::MuonEfficiencyScaleFactorAlg)
DECLARE_ALGORITHM_FACTORY (CP::MuonTriggerEfficiencyScaleFactorAlg)

DECLARE_FACTORY_ENTRIES(MuonAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::MuonCalibrationAndSmearingAlg)
  DECLARE_ALGORITHM (CP::MuonIsolationAlg)
  DECLARE_ALGORITHM (CP::MuonEfficiencyScaleFactorAlg)
  DECLARE_ALGORITHM (CP::MuonTriggerEfficiencyScaleFactorAlg)
}
