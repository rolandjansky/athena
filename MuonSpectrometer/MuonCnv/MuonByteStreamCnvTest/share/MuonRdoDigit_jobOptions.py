# job options fragments for the decoding :: muon digits --> muon RDO!

include.block("MuonByteStreamCnvTest/MuonRdoDigit_jobOptions.py")
print "DEPRECATION WARNING: please replace include(\"MuonByteStreamCnvTest/MuonRdoDigit_jobOptions.py\")\nwith:\nimport MuonCnvExample.MuonCablingConfig"


#load relevant libraries
theApp.Dlls += [ "MuonByteStreamCnvTest" ]

from AthenaCommon.DetFlags import DetFlags

import MuonCnvExample.MuonCablingConfig

#-----------------------------------RPC-------------------------------------
if DetFlags.haveRIO.RPC_on() or DetFlags.digitize.RPC_on() or DetFlags.haveRDO.RPC_on():
# FIXME temporary : avoid loading library in AtlasTrigger 
        if 'TriggerFlags' in dir() and TriggerFlags.doLVL1:     
           theApp.Dlls += [ "TrigT1RPChardware" ]
           theApp.Dlls += [ "TrigT1RPCsteering" ]
           theApp.Dlls += [ "TrigT1RPCmonitoring" ]

#--------------------------------------CSC-------------------------------------
if DetFlags.haveRIO.CSC_on() or DetFlags.digitize.CSC_on() or DetFlags.haveRDO.CSC_on():
        include ("CscCalibTools/CscCalibTool_jobOptions.py") # Is this needed? EJWM
