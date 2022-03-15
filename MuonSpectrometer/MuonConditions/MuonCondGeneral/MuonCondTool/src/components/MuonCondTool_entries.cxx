/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "../MDT_DCSConditionsTool.h"
#include "../MDT_DCSConditionsRun2Tool.h"
#include "../RpcDetectorStatusDbTool.h"
#include "../TGC_STATUSConditionsTool.h"
#include "../MDT_MapConversion.h"
#include "../CSC_DCSConditionsTool.h"
#include "../RPC_DCSConditionsTool.h"
#include "../MDT_DQConditionsTool.h"

DECLARE_COMPONENT( MDT_DCSConditionsTool )
DECLARE_COMPONENT( CSC_DCSConditionsTool )
DECLARE_COMPONENT( RPC_DCSConditionsTool )
DECLARE_COMPONENT( TGC_STATUSConditionsTool )
DECLARE_COMPONENT( RpcDetectorStatusDbTool )
DECLARE_COMPONENT( MDT_DQConditionsTool )
DECLARE_COMPONENT( MDT_MapConversion )
DECLARE_COMPONENT( MDT_DCSConditionsRun2Tool )
