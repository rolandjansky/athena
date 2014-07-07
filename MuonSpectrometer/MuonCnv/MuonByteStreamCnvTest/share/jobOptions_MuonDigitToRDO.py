# job options fragments for the decoding :: muon digits --> muon RDO!

from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if DetFlags.digitize.MDT_on():
	from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MdtDigitToMdtRDO
	topSequence += MdtDigitToMdtRDO( "MdtDigitToMdtRDO" )
if DetFlags.digitize.CSC_on():
        topSequence += CfgGetter.getAlgorithm("CscDigitToCscRDO/CscDigitToCscRDO", tryDefaultConfigurable=True)
if DetFlags.digitize.RPC_on():
        from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import RpcDigitToRpcRDO
        topSequence += RpcDigitToRpcRDO( "RpcDigitToRpcRDO" )
if DetFlags.digitize.TGC_on():
        from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import TgcDigitToTgcRDO
        topSequence += TgcDigitToTgcRDO( "TgcDigitToTgcRDO" )
