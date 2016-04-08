###############################################################
#
# Job options file to test SCT module distortions tool
#
#==============================================================


from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags import DetFlags

#from AthenaCommon.AppMgr import ServiceMgr

#GlobalFlags.DetGeo.set_ctbh8()
GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetGeo.set_commis()

GlobalFlags.DataSource.set_geant4()
#GlobalFlags.DataSource.set_data()

# Just the SCT
#DetFlags.detdescr.pixel_setOn()

DetFlags.detdescr.SCT_setOn()
DetFlags.SCT_setOn()

# Select the geometry version.
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-CSC-02-00-00'

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion



# Load algorithms Any algorithm that uses the tool will do
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from SCT_ModuleDistortions.SCT_ModuleDistortionsConf import SCT_TestDistortionsTool
topSequence+=SCT_TestDistortionsTool()
topSequence.SCT_TestDistortionsTool.OutputLevel = INFO

from SCT_ModuleDistortions.SCT_ModuleDistortionsConf import SCT_DistortionsTool
ToolSvc += SCT_DistortionsTool()
ToolSvc.SCT_DistortionsTool.OutputLevel = INFO

#ToolSvc.SCT_DistortionsTool.InputSource = 0  # None

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output =["disto DATAFILE='test.root' opt='RECREATE'"];

##--------------------------------------------------------------
## Load IOVDbSvc
##--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
##IOVDbSvc.GlobalTag="HEAD"
#
IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"

#==============================================================
#
# End of job options file
#
###############################################################


