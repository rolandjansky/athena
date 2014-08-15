#include "GaudiKernel/DeclareFactoryEntries.h"

#include "EvgenProdTools/CopyEventWeight.h"
#include "EvgenProdTools/CountHepMC.h"
#include "EvgenProdTools/FixHepMC.h"
#include "EvgenProdTools/TestHepMC.h"

DECLARE_ALGORITHM_FACTORY(CopyEventWeight)
DECLARE_ALGORITHM_FACTORY(CountHepMC)
DECLARE_ALGORITHM_FACTORY(FixHepMC)
DECLARE_ALGORITHM_FACTORY(TestHepMC)

DECLARE_FACTORY_ENTRIES( EvgenProdTools ) {
  DECLARE_ALGORITHM(CopyEventWeight)
  DECLARE_ALGORITHM(CountHepMC)
  DECLARE_ALGORITHM(FixHepMC)
  DECLARE_ALGORITHM(TestHepMC)
}
