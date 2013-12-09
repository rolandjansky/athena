#include "MuonRdoToPrepData/CscRdoToCscPrepData.h"
#include "MuonRdoToPrepData/MdtRdoToMdtPrepData.h"
#include "MuonRdoToPrepData/RpcRdoToRpcPrepData.h"
#include "MuonRdoToPrepData/TgcRdoToTgcPrepData.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( CscRdoToCscPrepData )
DECLARE_ALGORITHM_FACTORY( MdtRdoToMdtPrepData )
DECLARE_ALGORITHM_FACTORY( RpcRdoToRpcPrepData )
DECLARE_ALGORITHM_FACTORY( TgcRdoToTgcPrepData )

DECLARE_FACTORY_ENTRIES( MuonRdoToPrepData ) {
	DECLARE_ALGORITHM( CscRdoToCscPrepData )
	DECLARE_ALGORITHM( MdtRdoToMdtPrepData )
	DECLARE_ALGORITHM( RpcRdoToRpcPrepData )
	DECLARE_ALGORITHM( TgcRdoToTgcPrepData )
}


