#**************************************************************
#
# MTHelloWorldLvl1 example
#
#==============================================================
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *
from AthenaCommon import CfgMgr

#--------------------------------------------------------------
# disable sending of Level 2 result
#--------------------------------------------------------------
if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
    svcMgr += ByteStreamAddressProviderSvc()

svcMgr.ByteStreamAddressProviderSvc.OutputLevel = OUTPUTLEVEL
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    "ROIB::RoIBResult/RoIBResult"
    ]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
HelloWorld = CfgMgr.MTHelloWorldLvl1("HelloWorld")

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += HelloWorld

#--------------------------------------------------------------
# need to run LVL1 configuration for LVL1 conversion in HLTSSW
#--------------------------------------------------------------
svcMgr += CfgMgr.LVL1CTP__L1Config("L1Config")
svcMgr += CfgMgr.LVL1CTP__CTPHardware("CTPHardware")
L1Config = svcMgr.L1Config
CTPHardware = svcMgr.CTPHardware

if not ('TrigT1ConfigVersion' in dir()):
    TrigT1ConfigVersion="CSC-02"
    if 'DetDescrVersion' in dir():
        print DetDescrVersion
        # what follows is a poor-man's database (hopefully soon to be replaced by a real one)
        if DetDescrVersion.startswith("Rome"):
            TrigT1ConfigVersion="DC2"
        elif DetDescrVersion.startswith("DC2"):
            TrigT1ConfigVersion="DC2"
        elif DetDescrVersion.startswith("ATLAS-DC3"):
            TrigT1ConfigVersion="DC2"

L1Config.triggerMenuFileLocation   = "LVL1triggermenu"+TrigT1ConfigVersion+".xml"
L1Config.thresholdListFileLocation = "LVL1triggerthresholds"+TrigT1ConfigVersion+".xml"
L1Config.muctpiConfigLocation      = "trigger.muctpi.xml"
CTPHardware.hardwareXMLFile        = "hardware.xml"
CTPHardware.printoutLUT            = "no"

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#
#--- retrieve the Level-1 result
#
HelloWorld.RetrieveLvl1 = TRUE
#
#--- retrieve the ROBs over the network
#
HelloWorld.RetrieveROBs = TRUE
#--- empty list of ROBs
HelloWorld.RobId = [ ]

#--- Give a list of ROBs contained in your input file
# ------------------
#HelloWorld.RobId = [    
# 0x42002a,
# 0x42002b,
# 0x42002e,
# 0x42002f,
# 0x420060,
# 0x420064
# ]

ROBDataProviderSvc.OutputLevel = OUTPUTLEVEL

#==============================================================
#
# End of MTHelloWorldLvl1 example
#
#**************************************************************
