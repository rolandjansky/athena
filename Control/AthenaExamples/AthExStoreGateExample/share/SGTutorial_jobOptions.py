###############################################################
#
# Job options file
#
#==============================================================

#use McEventSelector
import AthenaCommon.AtlasUnixGeneratorJob

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#top algorithms to be run
job += CfgMgr.SGWrite()
job += CfgMgr.SGRead()

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = Lvl.DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

from AthenaCommon.AppMgr import theApp
# Number of events to be processed (default is 10)
theApp.EvtMax = 2

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
job.SGRead.DataObjKey = "MyData"
job.SGWrite.DataObjKey = "MyData"

#==============================================================
#
# End of job options file
#
###############################################################

