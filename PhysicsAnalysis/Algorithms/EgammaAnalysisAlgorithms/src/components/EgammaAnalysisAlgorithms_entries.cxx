// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <EgammaAnalysisAlgorithms/EgammaCalibrationAndSmearingAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::EgammaCalibrationAndSmearingAlg)

DECLARE_FACTORY_ENTRIES(EgammaAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::EgammaCalibrationAndSmearingAlg)
}
