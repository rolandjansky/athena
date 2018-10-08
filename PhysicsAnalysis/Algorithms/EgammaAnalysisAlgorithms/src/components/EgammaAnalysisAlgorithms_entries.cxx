// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <EgammaAnalysisAlgorithms/EgammaCalibrationAndSmearingAlg.h>
#include <EgammaAnalysisAlgorithms/EgammaIsGoodOQSelectionTool.h>
#include <EgammaAnalysisAlgorithms/EgammaIsolationCorrectionAlg.h>
#include <EgammaAnalysisAlgorithms/EgammaIsolationSelectionAlg.h>
#include <EgammaAnalysisAlgorithms/ElectronEfficiencyCorrectionAlg.h>
#include <EgammaAnalysisAlgorithms/PhotonEfficiencyCorrectionAlg.h>
#include <EgammaAnalysisAlgorithms/PhotonShowerShapeFudgeAlg.h>

DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, EgammaCalibrationAndSmearingAlg)
DECLARE_NAMESPACE_TOOL_FACTORY (CP, EgammaIsGoodOQSelectionTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, EgammaIsolationCorrectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, EgammaIsolationSelectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, ElectronEfficiencyCorrectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, PhotonEfficiencyCorrectionAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (CP, PhotonShowerShapeFudgeAlg)

DECLARE_FACTORY_ENTRIES(EgammaAnalysisAlgorithms) {
  DECLARE_NAMESPACE_ALGORITHM (CP, EgammaCalibrationAndSmearingAlg)
  DECLARE_NAMESPACE_TOOL (CP, EgammaIsGoodOQSelectionTool)
  DECLARE_NAMESPACE_ALGORITHM (CP, EgammaIsolationCorrectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, EgammaIsolationSelectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, ElectronEfficiencyCorrectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, PhotonEfficiencyCorrectionAlg)
  DECLARE_NAMESPACE_ALGORITHM (CP, PhotonShowerShapeFudgeAlg)
}
