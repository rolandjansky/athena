// AsgExampleTools_entries.cxx

#include <GaudiKernel/DeclareFactoryEntries.h>

#include <SystematicsHandles/SysListLoaderAlg.h>

DECLARE_ALGORITHM_FACTORY (SysListLoaderAlg)

DECLARE_FACTORY_ENTRIES(SystematicsHandles) {
  DECLARE_ALGORITHM (SysListLoaderAlg)
}

