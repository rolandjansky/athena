// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <FTagAnalysisAlgorithms/BTaggingEfficiencyAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::BTaggingEfficiencyAlg)

DECLARE_FACTORY_ENTRIES(FTagAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::BTaggingEfficiencyAlg)
}
