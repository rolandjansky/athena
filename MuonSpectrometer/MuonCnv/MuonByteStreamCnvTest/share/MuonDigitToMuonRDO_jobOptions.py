from AthenaCommon.DetFlags import DetFlags

#-----------------------------------MDT---------------------------------------
if DetFlags.haveRIO.MDT_on() or DetFlags.digitize.MDT_on():
	theApp.TopAlg += [ "MdtDigitToMdtRDO" ]

#--------------------------------------CSC------------------------------------
if DetFlags.haveRIO.CSC_on() or DetFlags.digitize.CSC_on():
	include( "MuonByteStreamCnvTest/CscDigitToCscRDO_jobOptions.py" )

#-----------------------------------RPC---------------------------------------
if DetFlags.haveRIO.RPC_on() or DetFlags.digitize.RPC_on():
	theApp.TopAlg += [ "RpcDigitToRpcRDO" ]

#--------------------------------------TGC------------------------------------
if DetFlags.haveRIO.TGC_on() or DetFlags.digitize.TGC_on():
	include( "MuonByteStreamCnvTest/TgcDigitToTgcRDO_jobOptions.py" )
