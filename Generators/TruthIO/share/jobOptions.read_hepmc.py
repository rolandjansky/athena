###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from TruthIO.TruthIOConf import HepMCReadFromFile
job += HepMCReadFromFile()

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel               = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 5
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------

re = job.HepMCReadFromFile
re.AsciiFile= "hepmc.out"

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "Stream1" )
Stream1.ItemList += [ "2101#*", "133273#*" ]
# This line is not really needed. Just to show that PoolSvc is already covered by the ServiceMgr
PoolSvc = ServiceMgr.PoolSvc
Stream1.OutputFile = "McEvent.root"

#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#==============================================================
#
# End of job options file
#
###############################################################
