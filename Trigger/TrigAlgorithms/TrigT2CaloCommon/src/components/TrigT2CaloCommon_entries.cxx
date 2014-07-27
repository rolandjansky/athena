//#include "TrigT2CaloCommon/T2CaloMon.h"
//#include "TrigT2CaloCommon/T2CaloL1Sim.h"
//#include "TrigT2CaloCommon/T2CaloMonTool.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "TrigT2CaloCommon/TrigDataAccessATLFAST.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigT2CaloCommon/T2CaloInitSvc.h"
#include "TrigT2CaloCommon/T2GeometryTool.h"

DECLARE_SERVICE_FACTORY( T2CaloInitSvc )

//DECLARE_ALGORITHM_FACTORY( T2CaloMon )
//DECLARE_TOOL_FACTORY( T2CaloL1Sim )
//DECLARE_TOOL_FACTORY( T2CaloMonTool )
DECLARE_TOOL_FACTORY( TrigDataAccess )
DECLARE_TOOL_FACTORY( TrigDataAccessATLFAST )
DECLARE_TOOL_FACTORY( T2GeometryTool )

DECLARE_FACTORY_ENTRIES(TrigT2CaloCommon) {
//    DECLARE_ALGORITHM( T2CaloMon );
//    DECLARE_TOOL( T2CaloL1Sim );
//    DECLARE_TOOL( T2CaloMonTool );
    DECLARE_TOOL( TrigDataAccess );
    DECLARE_TOOL( TrigDataAccessATLFAST );
    DECLARE_TOOL( T2GeometryTool );
    DECLARE_SERVICE( T2CaloInitSvc );
}
