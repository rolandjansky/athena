# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getMdtRdoToMdtDigit", "MdtRdoToMdtDigitOverlay")
addTool("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getRpcRdoToRpcDigit", "RpcRdoToRpcDigitOverlay")
addTool("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getTgcRdoToTgcDigit", "TgcRdoToTgcDigitOverlay")
addTool("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSTGC_RdoToDigit", "STGC_RdoToDigitOverlay")
addTool("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getMM_RdoToDigit", "MM_RdoToDigitOverlay")

