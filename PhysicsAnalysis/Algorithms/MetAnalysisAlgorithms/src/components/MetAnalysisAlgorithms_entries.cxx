// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <MetAnalysisAlgorithms/MetBuilderAlg.h>
#include <MetAnalysisAlgorithms/MetMakerAlg.h>
#include <MetAnalysisAlgorithms/MetSignificanceAlg.h>
#include <MetAnalysisAlgorithms/MetSystematicsAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::MetBuilderAlg)
DECLARE_ALGORITHM_FACTORY (CP::MetMakerAlg)
DECLARE_ALGORITHM_FACTORY (CP::MetSignificanceAlg)
DECLARE_ALGORITHM_FACTORY (CP::MetSystematicsAlg)

DECLARE_FACTORY_ENTRIES(MetAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::MetBuilderAlg)
  DECLARE_ALGORITHM (CP::MetMakerAlg)
  DECLARE_ALGORITHM (CP::MetSignificanceAlg)
  DECLARE_ALGORITHM (CP::MetSystematicsAlg)
}
