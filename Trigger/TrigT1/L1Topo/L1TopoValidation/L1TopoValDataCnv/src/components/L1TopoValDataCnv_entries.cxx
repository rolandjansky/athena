#include "GaudiKernel/DeclareFactoryEntries.h"

#ifndef XAOD_ANALYSIS
#include "../L1TopoValDataCnvAlg.h"
#endif

#ifndef XAOD_ANALYSIS
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, L1TopoValDataCnvAlg )
#endif

DECLARE_FACTORY_ENTRIES(L1TopoValDataCnv) {
#ifndef XAOD_ANALYSIS
    DECLARE_NAMESPACE_ALGORITHM( xAODMaker, L1TopoValDataCnvAlg )
#endif
}
