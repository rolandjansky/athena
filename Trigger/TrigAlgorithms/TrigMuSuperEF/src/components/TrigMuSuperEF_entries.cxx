#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigMuSuperEF.h"
#include "../MyAlg.h"

DECLARE_ALGORITHM_FACTORY(TrigMuSuperEF)
DECLARE_ALGORITHM_FACTORY(MyAlg)


DECLARE_FACTORY_ENTRIES(TrigMuSuperEF) {

  DECLARE_ALGORITHM( TrigMuSuperEF )
  DECLARE_ALGORITHM( MyAlg )

}

