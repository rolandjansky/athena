// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <JetAnalysisAlgorithms/JetCalibrationAlg.h>
#include <JetAnalysisAlgorithms/JetGhostMuonAssociationAlg.h>
#include <JetAnalysisAlgorithms/JetModifierAlg.h>
#include <JetAnalysisAlgorithms/JetSelectionAlg.h>
#include <JetAnalysisAlgorithms/JetSmearingAlg.h>
#include <JetAnalysisAlgorithms/JetUncertaintiesAlg.h>
#include <JetAnalysisAlgorithms/JvtEfficiencyAlg.h>
#include <JetAnalysisAlgorithms/JvtUpdateAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::JetCalibrationAlg)
DECLARE_ALGORITHM_FACTORY (CP::JetGhostMuonAssociationAlg)
DECLARE_ALGORITHM_FACTORY (CP::JetModifierAlg)
DECLARE_ALGORITHM_FACTORY (CP::JetSelectionAlg)
DECLARE_ALGORITHM_FACTORY (CP::JetSmearingAlg)
DECLARE_ALGORITHM_FACTORY (CP::JetUncertaintiesAlg)
DECLARE_ALGORITHM_FACTORY (CP::JvtEfficiencyAlg)
DECLARE_ALGORITHM_FACTORY (CP::JvtUpdateAlg)

DECLARE_FACTORY_ENTRIES(JetAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::JetCalibrationAlg)
  DECLARE_ALGORITHM (CP::JetGhostMuonAssociationAlg)
  DECLARE_ALGORITHM (CP::JetModifierAlg)
  DECLARE_ALGORITHM (CP::JetSelectionAlg)
  DECLARE_ALGORITHM (CP::JetSmearingAlg)
  DECLARE_ALGORITHM (CP::JetUncertaintiesAlg)
  DECLARE_ALGORITHM (CP::JvtEfficiencyAlg)
  DECLARE_ALGORITHM (CP::JvtUpdateAlg)
}
