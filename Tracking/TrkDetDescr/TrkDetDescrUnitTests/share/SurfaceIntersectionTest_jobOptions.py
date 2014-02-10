###############################################################
#
# Job options 
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# the global detflags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel
DetDescrVerstion = 'ATLAS-GEO-20-00-00'
from AtlasGeoModel import SetGeometryVersion 
from AtlasGeoModel import GeoModelInit 

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-SIM-00-00-00")

# load the displayer
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__RandomSurfaceBuilder
RandomSurfaceBuilder = Trk__RandomSurfaceBuilder(name= 'RandomSurfaceBuilder')
RandomSurfaceBuilder.OutputLevel          = INFO
RandomSurfaceBuilder.WorldDimensions = [ 7000., 7000., 15000. ]
ToolSvc += RandomSurfaceBuilder

# load the TTree writer
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc() 
ServiceMgr.THistSvc.Output = [ "val DATAFILE='SurfaceIntersectionTest.root' TYPE='ROOT' OPT='RECREATE'" ]

# Add top algorithms to be run
from TrkDetDescrUnitTests.TrkDetDescrUnitTestsConf import Trk__SurfaceIntersectionTest
SurfaceIntersectionTest = Trk__SurfaceIntersectionTest('SurfaceIntersectionTest')
SurfaceIntersectionTest.SurfaceBuilder = RandomSurfaceBuilder
SurfaceIntersectionTest.NumberOfTestsPerEvent   = 100
SurfaceIntersectionTest.NumberOfTestsPerSurface = 1000
SurfaceIntersectionTest.WriteTTree              = False
SurfaceIntersectionTest.OutputLevel             = INFO
job += SurfaceIntersectionTest   # 1 alg, named "SurfaceIntersectionTest"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

# Output level for HelloAlg only (note name: instance, not type)
SurfaceIntersectionTest = INFO

# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 50

#==============================================================
#
# End of job options file
#
###############################################################
