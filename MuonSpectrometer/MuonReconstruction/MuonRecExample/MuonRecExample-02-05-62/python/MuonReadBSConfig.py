# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Tools and Algorithms are now defined in module MuonCnvExample.MuonBSConfig via ConfiguredFactory
#
# This file is for backwards compat - FILE TO BE REMOVED as soon as all clients get these tools via AthenaCommon.CfgGetter
#
from AthenaCommon.CfgGetter import getPublicTool
if DetFlags.readRDOBS.MDT_on():
    MuonMdtRawDataProviderTool = getPublicTool("MdtRawDataProviderTool")
else:
    MuonMdtRawDataProviderTool = None

if DetFlags.readRDOBS.RPC_on():
    MuonRpcRawDataProviderTool = getPublicTool("RpcRawDataProviderTool")
else:
    MuonRpcRawDataProviderTool = None

if DetFlags.readRDOBS.TGC_on():
    MuonTgcRawDataProviderTool = getPublicTool("TgcRawDataProviderTool")
else:
    MuonTgcRawDataProviderTool = None

if DetFlags.readRDOBS.CSC_on():
    MuonCscRawDataProviderTool = getPublicTool("CscRawDataProviderTool")
else:
    MuonCscRawDataProviderTool = None
