#include "GaudiKernel/DeclareFactoryEntries.h"
#include "AmdcDb/AmdcDb.h"

DECLARE_SERVICE_FACTORY( AmdcDb )
 
DECLARE_FACTORY_ENTRIES(AmdcDb) {
    DECLARE_SERVICE( AmdcDb );
}
 
