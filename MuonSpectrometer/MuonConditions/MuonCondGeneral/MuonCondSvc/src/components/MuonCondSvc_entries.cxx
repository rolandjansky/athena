#include "MuonCondSvc/MuonAlignmentDbSvc.h"
//#include "MuonCondSvc/MuonDetectorStatusDbSvc.h"

#include "../CscCoolStrSvc.h"
#include "MuonCondSvc/MdtCoolStrSvc.h"
#include "MuonCondSvc/RpcCoolStrSvc.h"
#include "MuonCondSvc/RpcStatusDbSvc.h"

#include "MuonCondSvc/MuonAlignmentErrorDbSvc.h"
#include "MuonCondSvc/MDTCondSummarySvc.h"
#include "MuonCondSvc/RPCCondSummarySvc.h"
#include "MuonCondSvc/TGCCondSummarySvc.h"
#include "MuonCondSvc/CSCCondSummarySvc.h"
#include "MuonCondSvc/MDT_DCSConditionsSvc.h"
#include "MuonCondSvc/MDT_DeadTubeConditionsSvc.h"
#include "MuonCondSvc/MDT_DQConditionsSvc.h"
#include "MuonCondSvc/CSC_DCSConditionsSvc.h"
#include "MuonCondSvc/RPC_STATUSConditionsSvc.h"
#include "MuonCondSvc/TriggerCoolSvc.h"
#include "MuonCondSvc/TGC_STATUSConditionsSvc.h"
#include "MuonCondSvc/RPC_DCSConditionsSvc.h"
#include "MuonCondSvc/MDT_DCSConditionsRun2Svc.h"

using namespace MuonCalib;

DECLARE_SERVICE_FACTORY( MuonAlignmentDbSvc )
DECLARE_SERVICE_FACTORY( MuonAlignmentErrorDbSvc )
DECLARE_SERVICE_FACTORY( RpcStatusDbSvc )
DECLARE_SERVICE_FACTORY( CscCoolStrSvc )
DECLARE_SERVICE_FACTORY( MdtCoolStrSvc )
DECLARE_SERVICE_FACTORY( RpcCoolStrSvc )
DECLARE_SERVICE_FACTORY( MDT_DCSConditionsSvc )
DECLARE_SERVICE_FACTORY( CSC_DCSConditionsSvc )
DECLARE_SERVICE_FACTORY( MDTCondSummarySvc ) 
DECLARE_SERVICE_FACTORY( RPCCondSummarySvc )
DECLARE_SERVICE_FACTORY( TGCCondSummarySvc )
DECLARE_SERVICE_FACTORY( CSCCondSummarySvc )
DECLARE_SERVICE_FACTORY( MDT_DeadTubeConditionsSvc )
DECLARE_SERVICE_FACTORY( MDT_DQConditionsSvc )
DECLARE_SERVICE_FACTORY( RPC_STATUSConditionsSvc )
DECLARE_SERVICE_FACTORY( RPC_DCSConditionsSvc )
DECLARE_SERVICE_FACTORY( TriggerCoolSvc )
DECLARE_SERVICE_FACTORY( TGC_STATUSConditionsSvc )
DECLARE_SERVICE_FACTORY( MDT_DCSConditionsRun2Svc )

