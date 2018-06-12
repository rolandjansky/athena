// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <EgammaAnalysisAlgorithms/EgammaCalibrationAndSmearingAlg.h>
#include <EgammaAnalysisAlgorithms/EgammaIsolationCorrectionAlg.h>
#include <EgammaAnalysisAlgorithms/EgammaIsolationSelectionAlg.h>
#include <EgammaAnalysisAlgorithms/ElectronEfficiencyCorrectionAlg.h>
#include <EgammaAnalysisAlgorithms/PhotonEfficiencyCorrectionAlg.h>
#include <EgammaAnalysisAlgorithms/PhotonShowerShapeFudgeAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::EgammaCalibrationAndSmearingAlg)
DECLARE_ALGORITHM_FACTORY (CP::EgammaIsolationCorrectionAlg)
DECLARE_ALGORITHM_FACTORY (CP::EgammaIsolationSelectionAlg)
DECLARE_ALGORITHM_FACTORY (CP::ElectronEfficiencyCorrectionAlg)
DECLARE_ALGORITHM_FACTORY (CP::PhotonEfficiencyCorrectionAlg)
DECLARE_ALGORITHM_FACTORY (CP::PhotonShowerShapeFudgeAlg)

DECLARE_FACTORY_ENTRIES(EgammaAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::EgammaCalibrationAndSmearingAlg)
  DECLARE_ALGORITHM (CP::EgammaIsolationCorrectionAlg)
  DECLARE_ALGORITHM (CP::EgammaIsolationSelectionAlg)
  DECLARE_ALGORITHM (CP::ElectronEfficiencyCorrectionAlg)
  DECLARE_ALGORITHM (CP::PhotonEfficiencyCorrectionAlg)
  DECLARE_ALGORITHM (CP::PhotonShowerShapeFudgeAlg)
}
