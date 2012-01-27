###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## generator algorithm
job += CfgMgr.Pythia()

## algorithm to dump GenEvent's content on screen
job += CfgMgr.DumpMC()

svcMgr.MessageSvc.OutputLevel = Lvl.DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
svcMgr += CfgMgr.AtRndmGenSvc()
svcMgr.AtRndmGenSvc.Seeds = [
    "PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532"
    ]

Pythia = job.Pythia
Pythia.PythiaCommand = ["pysubs msel 13","pysubs ckin 3 18.","pypars mstp 43 2"]

# Number of events to be processed (default is 10)
theApp.EvtMax = 5

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool
job += CfgMgr.AthenaOutputStream(
    "OutStream",
    WritingTool = "AthenaPoolOutputStreamTool"
    )
job.OutStream.ItemList += [
    "EventInfo#*",
    "McEventCollection#*"
    ]
job.OutStream.OutputFile = "mc.event.pool"
#==============================================================
#
# End of job options file
#
###############################################################
