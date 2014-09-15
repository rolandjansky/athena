#include "GaudiKernel/DeclareFactoryEntries.h"

#include "RpcClusterization/RpcClusterBuilderPRD.h"

DECLARE_ALGORITHM_FACTORY( RpcClusterBuilderPRD )

DECLARE_FACTORY_ENTRIES(RpcClusterization) {
    DECLARE_ALGORITHM ( RpcClusterBuilderPRD )
}
