#include "TrigT2CaloCosmic/T2CaloCosmic.h"
//#include "TrigT2CaloCosmic/T2CaloCosmicMon.h"
//#include "TrigT2CaloCosmic/T2CaloCosmicSamp1.h"
#include "TrigT2CaloCosmic/CosmicSamp2Fex.h"
//#include "TrigT2CaloCosmic/CosmicSamp1Fex.h"
//#include "TrigT2CaloCosmic/CosmicEmEnFex.h"
#include "TrigT2CaloCosmic/CosmicHadEnFex.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( T2CaloCosmic )
//DECLARE_ALGORITHM_FACTORY( T2CaloCosmicSamp1 )
DECLARE_TOOL_FACTORY( CosmicSamp2Fex )
//DECLARE_TOOL_FACTORY( CosmicSamp1Fex )
//DECLARE_TOOL_FACTORY( CosmicEmEnFex )
DECLARE_TOOL_FACTORY( CosmicHadEnFex )
//DECLARE_TOOL_FACTORY( T2CaloCosmicMon )

DECLARE_FACTORY_ENTRIES(TrigT2CaloCosmic) {
    DECLARE_ALGORITHM( T2CaloCosmic );
//    DECLARE_ALGORITHM( T2CaloCosmicSamp1 );
    DECLARE_TOOL( CosmicSamp2Fex );
//    DECLARE_TOOL( CosmicSamp1Fex );
//    DECLARE_TOOL( CosmicEmEnFex );
    DECLARE_TOOL( CosmicHadEnFex );
//    DECLARE_TOOL( T2CaloCosmicMon );
}
