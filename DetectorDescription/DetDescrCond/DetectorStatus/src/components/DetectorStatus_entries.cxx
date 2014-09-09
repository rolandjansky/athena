#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../DetStatusSvc.h"
#include "DetectorStatus/DetStatusAlg.h"

DECLARE_SERVICE_FACTORY(DetStatusSvc)
DECLARE_ALGORITHM_FACTORY(DetStatusAlg)

DECLARE_FACTORY_ENTRIES(DetectorStatus) {
  DECLARE_SERVICE( DetStatusSvc );
  DECLARE_ALGORITHM( DetStatusAlg );
}
