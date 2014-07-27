
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigTileMuId/TrigTileRODMuAlg.h"
#include "TrigTileMuId/TrigTileLookForMuAlg.h"
#include "TrigTileMuId/TrigTileMuToNtuple.h"
#include "TrigTileMuId/TrigTileMonAlg.h"
//#include "TrigTileMuId/TrigTileMonTool.h"
#include "TrigTileMuId/TrigTileMuFex.h"

DECLARE_ALGORITHM_FACTORY( TrigTileRODMuAlg )
DECLARE_ALGORITHM_FACTORY( TrigTileLookForMuAlg )
DECLARE_ALGORITHM_FACTORY( TrigTileMuToNtuple )
DECLARE_ALGORITHM_FACTORY( TrigTileMonAlg )
DECLARE_ALGORITHM_FACTORY( TrigTileMuFex )
//DECLARE_TOOL_FACTORY( TrigTileMonTool )


DECLARE_FACTORY_ENTRIES(TrigTileMuId) {
    DECLARE_ALGORITHM( TrigTileRODMuAlg )
    DECLARE_ALGORITHM( TrigTileLookForMuAlg )
    DECLARE_ALGORITHM( TrigTileMuToNtuple )
    DECLARE_ALGORITHM( TrigTileMonAlg )
    DECLARE_ALGORITHM( TrigTileMuFex )
    //DECLARE_ALGTOOL( TrigTileMonTool )

}

//Notes:
//
//1. The argument to the DECLARE_FACTORY_ENTRIES(XXX) is the name of the
//   component library (libXXX.so).
//2. Each Algorithm that is contained in the library should be declared
//   by the DECLARE_ALGORITHM() statement.
//3. Each Service that is contained in the library should be declared
//   by the DECLARE_SERVICE() statement.
// 
// See Athena User Guide for more information
