###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 5
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = ["mc.event.pool"]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm"
#top algorithms to be run, and the libraries that house them
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += CfgMgr.DumpMC("DumpMC")

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool
job += CfgMgr.AthenaOutputStream(
    "OutStream",
    WritingTool = "AthenaPoolOutputStreamTool"
    )
job.OutStream.ItemList += [
    "EventInfo#*",
    "McEventCollection#*"
    ]
job.OutStream.OutputFile = "reaccessed.mc.event.pool"

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 2
#==============================================================
#
# End of job options file
#
###############################################################
