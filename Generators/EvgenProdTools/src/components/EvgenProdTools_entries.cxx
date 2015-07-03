#include "GaudiKernel/DeclareFactoryEntries.h"

#ifndef XAOD_ANALYSIS
#include "EvgenProdTools/CopyEventWeight.h"
#include "EvgenProdTools/CountHepMC.h"
#include "EvgenProdTools/FixHepMC.h"
#include "EvgenProdTools/TestHepMC.h"
#include "EvgenProdTools/EvgenOTFTopUpSvc.h"

DECLARE_ALGORITHM_FACTORY(CopyEventWeight)
DECLARE_ALGORITHM_FACTORY(CountHepMC)
DECLARE_ALGORITHM_FACTORY(FixHepMC)
DECLARE_ALGORITHM_FACTORY(TestHepMC)
DECLARE_SERVICE_FACTORY(EvgenOTFTopUpSvc)
#endif

#include "../HepMCWeightSvc.h"
DECLARE_SERVICE_FACTORY(HepMCWeightSvc)

DECLARE_FACTORY_ENTRIES( EvgenProdTools ) {
#ifndef XAOD_ANALYSIS
  DECLARE_ALGORITHM( ExpressionGenFilter )
  DECLARE_ALGORITHM(CopyEventWeight)
  DECLARE_ALGORITHM(CountHepMC)
  DECLARE_ALGORITHM(FixHepMC)
  DECLARE_ALGORITHM(TestHepMC)
  DECLARE_SERVICE(EvgenOTFTopUpSvc)
#endif
  DECLARE_SERVICE(HepMCWeightSvc)
}
