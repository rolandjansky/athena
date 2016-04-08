#include "TrigT2CaloEgamma/T2CaloEgamma.h"
//#include "TrigT2CaloEgamma/T2CaloEgammaMon.h"
//#include "TrigT2CaloEgamma/T2CaloEgammaSamp1.h"
#include "TrigT2CaloEgamma/EgammaSamp2Fex.h"
#include "TrigT2CaloEgamma/EgammaSamp1Fex.h"
#include "TrigT2CaloEgamma/EgammaEmEnFex.h"
#include "TrigT2CaloEgamma/EgammaHadEnFex.h"
#include "TrigT2CaloEgamma/RingerFex.h"
//#include "TrigT2CaloEgamma/T2CaloSwSeed.h"
//#include "TrigT2CaloEgamma/T2CaloSwCluster.h"
#include "TrigT2CaloEgamma/EgammaAllFex.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( T2CaloEgamma )
//DECLARE_ALGORITHM_FACTORY( T2CaloEgammaSamp1 )
//DECLARE_ALGORITHM_FACTORY( T2CaloSwSeed )
//DECLARE_ALGORITHM_FACTORY( T2CaloSwCluster )
DECLARE_TOOL_FACTORY( EgammaSamp2Fex )
DECLARE_TOOL_FACTORY( EgammaSamp1Fex )
DECLARE_TOOL_FACTORY( EgammaEmEnFex )
DECLARE_TOOL_FACTORY( EgammaHadEnFex )
DECLARE_TOOL_FACTORY( RingerFex )
DECLARE_TOOL_FACTORY( EgammaAllFex )
//DECLARE_TOOL_FACTORY( T2CaloEgammaMon )

DECLARE_FACTORY_ENTRIES(TrigT2CaloEgamma) {
    DECLARE_ALGORITHM( T2CaloEgamma );
    //DECLARE_ALGORITHM( T2CaloEgammaSamp1 );
    //DECLARE_ALGORITHM( T2CaloSwSeed );
    //DECLARE_ALGORITHM( T2CaloSwCluster );
    DECLARE_TOOL( EgammaSamp2Fex );
    DECLARE_TOOL( EgammaSamp1Fex );
    DECLARE_TOOL( EgammaEmEnFex );
    DECLARE_TOOL( EgammaHadEnFex );
    DECLARE_TOOL( RingerFex );
//    DECLARE_TOOL( T2CaloEgammaMon );
}
