# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm

addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getMdtRdoToMdtDigitAlg", "MdtRdoToMdtDigitOverlayAlg")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getRpcRdoToRpcDigitAlg", "RpcRdoToRpcDigitOverlayAlg")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getTgcRdoToTgcDigitAlg", "TgcRdoToTgcDigitOverlayAlg")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSTGC_RdoToDigitAlg", "STGC_RdoToDigitOverlayAlg")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getMM_RdoToDigitAlg", "MM_RdoToDigitOverlayAlg")

addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getMdtDigitToMdtRDO" , "MdtDigitToMdtRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getRpcDigitToRpcRDO" , "RpcDigitToRpcRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getTgcDigitToTgcRDO" , "TgcDigitToTgcRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSTGC_DigitToRDO" , "STGC_DigitToRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getMM_DigitToRDO" , "MM_DigitToRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSigMdtDigitToMdtRDO" , "SigMdtDigitToMdtRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSigRpcDigitToRpcRDO" , "SigRpcDigitToRpcRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSigTgcDigitToTgcRDO" , "SigTgcDigitToTgcRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getOverlayCscDigitToCscRDO" , "OverlayCscDigitToCscRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getOverlayMdtDigitToMdtRDO" , "OverlayMdtDigitToMdtRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getOverlayRpcDigitToRpcRDO" , "OverlayRpcDigitToRpcRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getOverlayTgcDigitToTgcRDO" , "OverlayTgcDigitToTgcRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getOverlaySTGC_DigitToRDO" , "OverlaySTGC_DigitToRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getOverlayMM_DigitToRDO" , "OverlayMM_DigitToRDO")
