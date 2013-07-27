#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCondCool/MdtReadWriteCoolStr.h"
#include "MuonCondCool/RpcReadWriteCoolStr.h"
#include "../CscReadWriteCoolStr.h"
#include "../CscCoolTest.h"
#include "MuonCondCool/MdtTubeCondCool.h"
#include "MuonCondCool/TriggerCool.h"
using namespace MuonCalib;

DECLARE_ALGORITHM_FACTORY( MdtReadWriteCoolStr )
DECLARE_ALGORITHM_FACTORY( RpcReadWriteCoolStr )
DECLARE_ALGORITHM_FACTORY( MdtTubeCondCool )
DECLARE_ALGORITHM_FACTORY( CscReadWriteCoolStr )
DECLARE_ALGORITHM_FACTORY( CscCoolTest )
DECLARE_ALGORITHM_FACTORY( TriggerCool )

DECLARE_FACTORY_ENTRIES( MuonCondCool ){
   DECLARE_ALGORITHM( MdtReadWriteCoolStr )
   DECLARE_ALGORITHM( RpcReadWriteCoolStr )
   DECLARE_ALGORITHM( MdtTubeCondCool )
   DECLARE_ALGORITHM( CscReadWriteCoolStr )
   DECLARE_ALGORITHM( CscCoolTest )
   DECLARE_ALGORITHM( TriggerCool )

}
