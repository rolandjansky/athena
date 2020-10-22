// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <EgammaAnalysisAlgorithms/EgammaCalibrationAndSmearingAlg.h>
#include <EgammaAnalysisAlgorithms/EgammaIsGoodOQSelectionTool.h>
#include <EgammaAnalysisAlgorithms/EgammaIsolationCorrectionAlg.h>
#include <EgammaAnalysisAlgorithms/EgammaIsolationSelectionAlg.h>
#include <EgammaAnalysisAlgorithms/ElectronEfficiencyCorrectionAlg.h>
#include <EgammaAnalysisAlgorithms/PhotonEfficiencyCorrectionAlg.h>
#include <EgammaAnalysisAlgorithms/PhotonShowerShapeFudgeAlg.h>

DECLARE_COMPONENT (CP::EgammaCalibrationAndSmearingAlg)
DECLARE_COMPONENT (CP::EgammaIsGoodOQSelectionTool)
DECLARE_COMPONENT (CP::EgammaIsolationCorrectionAlg)
DECLARE_COMPONENT (CP::EgammaIsolationSelectionAlg)
DECLARE_COMPONENT (CP::ElectronEfficiencyCorrectionAlg)
DECLARE_COMPONENT (CP::PhotonEfficiencyCorrectionAlg)
DECLARE_COMPONENT (CP::PhotonShowerShapeFudgeAlg)

