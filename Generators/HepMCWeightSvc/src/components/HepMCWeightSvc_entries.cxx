#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../HepMCWeightSvc.h"
DECLARE_SERVICE_FACTORY(HepMCWeightSvc)

DECLARE_FACTORY_ENTRIES( HepMCWeightSvc ) {
  DECLARE_SERVICE(HepMCWeightSvc)
}
