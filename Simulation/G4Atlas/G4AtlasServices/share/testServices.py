#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
# Add top algorithms to be run
from G4AtlasServices.G4AtlasServicesConf import DetectorGeometrySvc
dgService=DetectorGeometrySvc()

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
dgService.OutputLevel = INFO

theApp.CreateSvc += ["DetectorGeometrySvc"]
svcMgr += dgService
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1
#==============================================================
