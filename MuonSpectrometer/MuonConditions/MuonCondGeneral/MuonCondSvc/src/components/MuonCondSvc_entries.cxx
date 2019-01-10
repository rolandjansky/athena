#include "MuonCondSvc/MuonAlignmentDbSvc.h"
//#include "MuonCondSvc/MuonDetectorStatusDbSvc.h"

#include "../CscCoolStrSvc.h"
#include "MuonCondSvc/MdtCoolStrSvc.h"
#include "MuonCondSvc/RpcCoolStrSvc.h"
#include "MuonCondSvc/RpcStatusDbSvc.h"

#include "MuonCondSvc/MuonAlignmentErrorDbAlg.h"
#include "MuonCondSvc/TGCTriggerDbAlg.h"
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

DECLARE_COMPONENT( MuonAlignmentDbSvc )
DECLARE_COMPONENT( MuonAlignmentErrorDbAlg )
DECLARE_COMPONENT( TGCTriggerDbAlg )
DECLARE_COMPONENT( RpcStatusDbSvc )
DECLARE_COMPONENT( CscCoolStrSvc )
DECLARE_COMPONENT( MdtCoolStrSvc )
DECLARE_COMPONENT( RpcCoolStrSvc )
DECLARE_COMPONENT( MDT_DCSConditionsSvc )
DECLARE_COMPONENT( CSC_DCSConditionsSvc )
DECLARE_COMPONENT( MDTCondSummarySvc ) 
DECLARE_COMPONENT( RPCCondSummarySvc )
DECLARE_COMPONENT( TGCCondSummarySvc )
DECLARE_COMPONENT( CSCCondSummarySvc )
DECLARE_COMPONENT( MDT_DeadTubeConditionsSvc )
DECLARE_COMPONENT( MDT_DQConditionsSvc )
DECLARE_COMPONENT( RPC_STATUSConditionsSvc )
DECLARE_COMPONENT( RPC_DCSConditionsSvc )
DECLARE_COMPONENT( TriggerCoolSvc )
DECLARE_COMPONENT( TGC_STATUSConditionsSvc )
DECLARE_COMPONENT( MDT_DCSConditionsRun2Svc )

