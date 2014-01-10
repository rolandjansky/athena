from AthenaCommon.AlgSequence import AlgSequence 
job = AlgSequence() 
 
from AthenaCommon import CfgGetter 
from Digitization.DigitizationFlags import jobproperties 
if jobproperties.Digitization.doXingByXingPileUp(): # PileUpTool approach  
    job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("RpcDigitizationTool", checkType=True) ] 
else : # Algorithm approach 
    job += CfgGetter.getAlgorithm("RPC_Digitizer/RPC_Digitizer", tryDefaultConfigurable=True)  








###
###from Digitization.DigitizationFlags import jobproperties
###
###include ("RPC_Digitization/RpcDigitizationTool_jobOptions.py")
###
###from AthenaCommon.AlgSequence import AlgSequence
###job = AlgSequence()
####
#### this will create one digitization with all default properties
####
###from RPC_Digitization.RPC_DigitizationConf import RPC_Digitizer
###job += RPC_Digitizer("RPC_Digitizer")
###rpc = job.RPC_Digitizer # shortcut
###rpc.RPC_DigitizationTool = RpcDigitizationTool
###
#### hook for clients
###digitizer = rpc
###
#### cleanup
###del rpc

