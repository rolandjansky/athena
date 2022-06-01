include.block ("MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py")

from AthenaCommon.CfgGetter import getPublicTool

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

## configure the tool

from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigitTool
MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool (
                         DecodeMdtRDO = True,
                         DecodeRpcRDO = True,
                         DecodeTgcRDO = True,
                         DecodeCscRDO = MuonGeometryFlags.hasCSC(),
                         DecodeSTGC_RDO = MuonGeometryFlags.hasSTGC(),
                         DecodeMM_RDO = MuonGeometryFlags.hasMM(),
                         cscRdoDecoderTool=("Muon::CscRDO_Decoder" if MuonGeometryFlags.hasCSC() else ""),
                         stgcRdoDecoderTool=("Muon::STGC_RDO_Decoder" if MuonGeometryFlags.hasSTGC() else ""),
                         mmRdoDecoderTool=("Muon::MM_RDO_Decoder" if MuonGeometryFlags.hasMM() else "")
                         )
			 
if MuonGeometryFlags.hasCSC (): MuonRdoToMuonDigitTool.cscCalibTool = getPublicTool("CscCalibTool")
if MuonGeometryFlags.hasSTGC(): MuonRdoToMuonDigitTool.stgcRdoDecoderTool.CalibrationTool = getPublicTool("NSWCalibTool")
if MuonGeometryFlags.hasMM  (): MuonRdoToMuonDigitTool.mmRdoDecoderTool  .CalibrationTool = getPublicTool("NSWCalibTool")

ToolSvc += MuonRdoToMuonDigitTool

## configure the algorithm
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigit
topSequence += MuonRdoToMuonDigit( "MuonRdoToMuonDigit" )
topSequence.MuonRdoToMuonDigit.MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool

