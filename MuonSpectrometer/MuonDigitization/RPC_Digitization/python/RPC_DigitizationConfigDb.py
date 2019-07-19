# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm

addTool("RPC_Digitization.RPC_DigitizationConfig.RpcDigitizationTool" , "RpcDigitizationTool")
addTool("RPC_Digitization.RPC_DigitizationConfig.getRpcRange"         , "RpcRange")
addTool("RPC_Digitization.RPC_DigitizationConfig.Rpc_OverlayDigitizationTool" , "Rpc_OverlayDigitizationTool")
addAlgorithm("RPC_Digitization.RPC_DigitizationConfig.getRPC_OverlayDigitizer" , "RPC_OverlayDigitizer")
