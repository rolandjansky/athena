// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <TauAnalysisAlgorithms/DiTauSmearingAlg.h>
#include <TauAnalysisAlgorithms/TauEfficiencyCorrectionsAlg.h>
#include <TauAnalysisAlgorithms/TauSmearingAlg.h>
#include <TauAnalysisAlgorithms/TauTruthMatchingAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::DiTauSmearingAlg)
DECLARE_ALGORITHM_FACTORY (CP::TauEfficiencyCorrectionsAlg)
DECLARE_ALGORITHM_FACTORY (CP::TauSmearingAlg)
DECLARE_ALGORITHM_FACTORY (CP::TauTruthMatchingAlg)

DECLARE_FACTORY_ENTRIES(TauAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::DiTauSmearingAlg)
  DECLARE_ALGORITHM (CP::TauEfficiencyCorrectionsAlg)
  DECLARE_ALGORITHM (CP::TauSmearingAlg)
  DECLARE_ALGORITHM (CP::TauTruthMatchingAlg)
}
