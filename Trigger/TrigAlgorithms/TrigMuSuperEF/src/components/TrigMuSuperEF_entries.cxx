#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigMuSuperEF.h"
#include "../TrigMuonEFTagandProbe.h"

DECLARE_ALGORITHM_FACTORY(TrigMuSuperEF)
DECLARE_ALGORITHM_FACTORY(TrigMuonEFTagandProbe)


DECLARE_FACTORY_ENTRIES(TrigMuSuperEF) {

  DECLARE_ALGORITHM( TrigMuSuperEF )
  DECLARE_ALGORITHM( TrigMuonEFTagandProbe )

}

