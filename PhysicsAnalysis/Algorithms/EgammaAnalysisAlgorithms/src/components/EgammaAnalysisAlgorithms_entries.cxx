// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <EgammaAnalysisAlgorithms/EgammaCalibrationAndSmearingAlg.h>
#include <EgammaAnalysisAlgorithms/EgammaIsolationSelectionAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::EgammaCalibrationAndSmearingAlg)
DECLARE_ALGORITHM_FACTORY (CP::EgammaIsolationSelectionAlg)

DECLARE_FACTORY_ENTRIES(EgammaAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::EgammaCalibrationAndSmearingAlg)
  DECLARE_ALGORITHM (CP::EgammaIsolationSelectionAlg)
}
