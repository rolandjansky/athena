#include "TrigT2CaloTau/T2CaloTau.h"
#include "TrigT2CaloTau/TauAllCaloDRFex.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( T2CaloTau )

DECLARE_TOOL_FACTORY( TauAllCaloDRFex )

DECLARE_FACTORY_ENTRIES(TrigT2CaloTau) {
    DECLARE_ALGORITHM( T2CaloTau );
    DECLARE_TOOL( TauAllCaloDRFex );
}
