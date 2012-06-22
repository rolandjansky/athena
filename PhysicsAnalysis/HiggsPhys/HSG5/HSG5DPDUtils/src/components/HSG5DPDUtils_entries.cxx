#include "GaudiKernel/DeclareFactoryEntries.h"
#include "HSG5DPDUtils/LeptonFilter.h"
#include "HSG5DPDUtils/LeptonJetsFilter.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( HSG5, LeptonFilter )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( HSG5, LeptonJetsFilter )

DECLARE_FACTORY_ENTRIES(HSG5DPDUtils) {
  DECLARE_NAMESPACE_ALGORITHM( HSG5, LeptonFilter );
  DECLARE_NAMESPACE_ALGORITHM( HSG5, LeptonJetsFilter );
}
