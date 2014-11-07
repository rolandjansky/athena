include.block("MuonRecExample/MuonDigitization_jobOptions.py")

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon import CfgMgr
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# contents below copied from MuonDigiExample/G4MuonDigit_jobOptions.py

if DetFlags.digitize.CSC_on():
    include( "CSC_Digitization/CSC_Digitization_jobOptions.py" )

if DetFlags.digitize.MDT_on():
    include( "MDT_Digitization/MDT_Digitization_jobOptions.py" )

if DetFlags.digitize.TGC_on():
    include( "TGC_Digitization/TGC_Digitization_jobOptions.py" )

if DetFlags.digitize.RPC_on():
    include( "RPC_Digitization/RPC_Digitization_jobOptions.py" )

if DetFlags.digitize.Micromegas_on():
    CfgGetter.getAlgorithm("MM_FastDigitizer/MM_FastDigitizer",tryDefaultConfigurable=True)


# contents below inspired by MuonByteStreamCnvTest/MuonDigitToMuonRDO_jobOptions.py
if DetFlags.digitize.CSC_on():
    include ("MuonCSC_CnvTools/CscDigitToCscRDOTool_jobOptions.py")
    job += CfgMgr.CscDigitToCscRDO( CscDigitToRDOTool = ToolSvc.CscDigitToCscRDOTool )

if DetFlags.digitize.MDT_on():
    job += CfgMgr.MdtDigitToMdtRDO()

if DetFlags.digitize.TGC_on():
    job += CfgMgr.TgcDigitToTgcRDO(isNewTgcDigit = False)

if DetFlags.digitize.RPC_on():
    job += CfgMgr.RpcDigitToRpcRDO()

