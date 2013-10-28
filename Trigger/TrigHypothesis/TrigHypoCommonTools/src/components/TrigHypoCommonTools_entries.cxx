#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigHypoCommonTools/L1InfoHypo.hxx"


DECLARE_ALGORITHM_FACTORY(L1InfoHypo)

DECLARE_FACTORY_ENTRIES(TrigHypoCommonTools) {
  DECLARE_ALGORITHM(L1InfoHypo);
}
