#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigL2MissingET/T2MissingET.h"
// #include "TrigL2MissingET/T2MissingETMuon.h"
#include "TrigL2MissingET/T2CaloMissingET.h"


using namespace PESA;

DECLARE_ALGORITHM_FACTORY( T2MissingET )
DECLARE_ALGORITHM_FACTORY( T2CaloMissingET )
// DECLARE_ALGORITHM_FACTORY( T2MissingETMuon )

DECLARE_FACTORY_ENTRIES( TrigL2MissingET ) {
    DECLARE_ALGORITHM( T2MissingET )
    DECLARE_ALGORITHM( T2CaloMissingET )
//     DECLARE_ALGORITHM( T2MissingETMuon )
}
