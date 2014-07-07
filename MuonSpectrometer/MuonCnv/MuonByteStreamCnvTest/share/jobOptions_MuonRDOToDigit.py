include.block ("MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py")

from AthenaCommon.CfgGetter import getPublicTool

## configure the tool

from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigitTool
MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool (
                         DecodeMdtRDO = True,
                         DecodeRpcRDO = True,
                         DecodeTgcRDO = True,
                         DecodeCscRDO = True ) 
			 
MuonRdoToMuonDigitTool.cscCalibTool = getPublicTool("CscCalibTool")

ToolSvc += MuonRdoToMuonDigitTool

## configure the algorithm
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigit
topSequence += MuonRdoToMuonDigit( "MuonRdoToMuonDigit" )
topSequence.MuonRdoToMuonDigit.MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool

