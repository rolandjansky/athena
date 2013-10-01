#include "../SgStressProducer.h"
#include "../SgStressConsumer.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
  
DECLARE_ALGORITHM_FACTORY( SgStressProducer )
DECLARE_ALGORITHM_FACTORY( SgStressConsumer )

DECLARE_FACTORY_ENTRIES( StoreGateTests ) {

  DECLARE_ALGORITHM( SgStressProducer )
  DECLARE_ALGORITHM( SgStressConsumer )
}
