/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonCondSvc/CscCoolStrSvc.h"
#include "MuonCondSvc/RpcCoolStrSvc.h"
#include "MuonCondSvc/RpcStatusDbSvc.h"

#include "MuonCondSvc/TGCTriggerDbAlg.h"
#include "MuonCondSvc/RPCCondSummarySvc.h"
#include "MuonCondSvc/TGCCondSummarySvc.h"
#include "MuonCondSvc/CSCCondSummarySvc.h"
#include "MuonCondSvc/CSC_DCSConditionsSvc.h"
#include "MuonCondSvc/RPC_STATUSConditionsSvc.h"
#include "MuonCondSvc/TriggerCoolSvc.h"
#include "MuonCondSvc/TGC_STATUSConditionsSvc.h"
#include "MuonCondSvc/RPC_DCSConditionsSvc.h"

using namespace MuonCalib;

DECLARE_COMPONENT( TGCTriggerDbAlg )
DECLARE_COMPONENT( RpcStatusDbSvc )
DECLARE_COMPONENT( CscCoolStrSvc )
DECLARE_COMPONENT( RpcCoolStrSvc )
DECLARE_COMPONENT( CSC_DCSConditionsSvc ) 
DECLARE_COMPONENT( RPCCondSummarySvc )
DECLARE_COMPONENT( TGCCondSummarySvc )
DECLARE_COMPONENT( CSCCondSummarySvc )
DECLARE_COMPONENT( RPC_STATUSConditionsSvc )
DECLARE_COMPONENT( RPC_DCSConditionsSvc )
DECLARE_COMPONENT( TriggerCoolSvc )
DECLARE_COMPONENT( TGC_STATUSConditionsSvc )
