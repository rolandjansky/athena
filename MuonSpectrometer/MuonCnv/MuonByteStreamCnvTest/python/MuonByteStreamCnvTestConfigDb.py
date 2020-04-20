# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm

addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getMdtRdoToMdtDigitAlg", "MdtRdoToMdtDigitAlg")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getRpcRdoToRpcDigitAlg", "RpcRdoToRpcDigitAlg")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getTgcRdoToTgcDigitAlg", "TgcRdoToTgcDigitAlg")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSTGC_RdoToDigitAlg", "STGC_RdoToDigitAlg")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getMM_RdoToDigitAlg", "MM_RdoToDigitAlg")

addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getMdtDigitToMdtRDO" , "MdtDigitToMdtRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getRpcDigitToRpcRDO" , "RpcDigitToRpcRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getTgcDigitToTgcRDO" , "TgcDigitToTgcRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSTGC_DigitToRDO" , "STGC_DigitToRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getMM_DigitToRDO" , "MM_DigitToRDO")

addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSigMdtDigitToMdtRDO" , "SigMdtDigitToMdtRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSigRpcDigitToRpcRDO" , "SigRpcDigitToRpcRDO")
addAlgorithm("MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig.getSigTgcDigitToTgcRDO" , "SigTgcDigitToTgcRDO")
