#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonRPC_Cabling/MuonRPC_CablingSvc.h"
#include "MuonRPC_Cabling/MuonRpcCablingTest.h"

DECLARE_SERVICE_FACTORY(MuonRPC_CablingSvc)
DECLARE_ALGORITHM_FACTORY( MuonRpcCablingTest )

DECLARE_FACTORY_ENTRIES(MuonRPC_Cabling) {
    DECLARE_SERVICE(MuonRPC_CablingSvc)
    DECLARE_ALGORITHM(MuonRpcCablingTest)
}

