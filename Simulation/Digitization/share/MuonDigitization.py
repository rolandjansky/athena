from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
##################################################################
#Digit -> RDO Conversion
##################################################################
if DetFlags.digitize.Muon_on():
    # Convert Digits to RDOs
    #----------------
    import MuonCnvExample.MuonCablingConfig
    #----------------
    if DetFlags.digitize.CSC_on():
        job += CfgGetter.getAlgorithm("CscDigitToCscRDO/CscDigitToCscRDO", tryDefaultConfigurable=True)
    #----------------
    if DetFlags.digitize.MDT_on():
        job += CfgGetter.getAlgorithm("MdtDigitToMdtRDO/MdtDigitToMdtRDO", tryDefaultConfigurable=True)
    #----------------
    if DetFlags.digitize.RPC_on():
        job += CfgGetter.getAlgorithm("RpcDigitToRpcRDO/RpcDigitToRpcRDO", tryDefaultConfigurable=True)
    #----------------
    if DetFlags.digitize.TGC_on():
        job += CfgGetter.getAlgorithm("TgcDigitToTgcRDO/TgcDigitToTgcRDO", tryDefaultConfigurable=True)
    #----------------
    if DetFlags.digitize.sTGC_on():
        job += CfgGetter.getAlgorithm("STGC_DigitToRDO/STGC_DigitToRDO", tryDefaultConfigurable=True)
    #----------------
    #if DetFlags.digitize.Micromegas_on():
    #    job += CfgGetter.getAlgorithm("MM_DigitToRDO/MM_DigitToRDO", tryDefaultConfigurable=True)
    #----------------
#endif digitize.Muon_on
