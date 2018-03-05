// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <AsgAnalysisAlgorithms/SysListLoaderAlg.h>

DECLARE_ALGORITHM_FACTORY (CP::SysListLoaderAlg)

DECLARE_FACTORY_ENTRIES(AsgAnalysisAlgorithms) {
  DECLARE_ALGORITHM (CP::SysListLoaderAlg)
}

