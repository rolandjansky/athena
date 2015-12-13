###############################################################
#
# Job options 
#
# This is for validation of ITk tracking geometry.
# Modified version of Tracking/TrkDetDescr/TrkDetDescrUnitTests/share/TrackingGeometryTest_jobOptions.py. (TrkDetDescrUnitTests-00-01-05)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/UpgradeSimulationInnerTrackerMigrationRel20p3p3#Validation_of_database_files
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
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# build GeoModel
if 'DetDescrVersion' not in dir():
    DetDescrVersion = 'ATLAS-P2-ITK-01-00-00'
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import SetGeometryVersion 
from AtlasGeoModel import GeoModelInit 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = DetDescrVersion

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-MC12-ITK-26-80-25")

# switch the material loading off
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialDatabaseLocal    = True
if TrkDetFlags.MaterialDatabaseLocal() is True :
    TrkDetFlags.MaterialSource           = 'COOL'
    TrkDetFlags.MaterialVersion          = 17
    TrkDetFlags.MaterialSubVersion       = ""
    # splitGeo = DetDescrVersion.split('-')
    # TrkDetFlags.MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2]
    TrkDetFlags.MaterialMagicTag = DetDescrVersion
    TrkDetFlags.MaterialStoreGateKey        = '/GLOBAL/TrackingGeo/LayerMaterialITK'
    TrkDetFlags.MaterialDatabaseLocalPath    = './'
    TrkDetFlags.MaterialDatabaseLocalName    = 'AtlasLayerMaterial-'+DetDescrVersion+'.db'

TrkDetFlags.ConfigurationOutputLevel = VERBOSE

TrkDetFlags.SLHC_Geometry = True

TrkDetFlags.PixelBuildingOutputLevel     = VERBOSE
TrkDetFlags.SCT_BuildingOutputLevel      = VERBOSE
TrkDetFlags.TRT_BuildingOutputLevel      = VERBOSE
TrkDetFlags.InDetBuildingOutputLevel     = VERBOSE
TrkDetFlags.ConfigurationOutputLevel     = VERBOSE


# load the tracking geometry service
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

# the processors
GeometryProcessors = []

# load the displayer
from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__RecursiveGeometryProcessor
RecursiveGeometryProcessor = Trk__RecursiveGeometryProcessor(name= 'RecursiveGeometryProcessor')
RecursiveGeometryProcessor.OutputLevel          = VERBOSE
ToolSvc += RecursiveGeometryProcessor
GeometryProcessors += [ RecursiveGeometryProcessor ]

# load the displayer
from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__TrackingVolumeDisplayer
TrackingVolumeDisplayer = Trk__TrackingVolumeDisplayer(name= 'TrackingVolumeDisplayer')
TrackingVolumeDisplayer.OutputLevel          = VERBOSE
ToolSvc += TrackingVolumeDisplayer
GeometryProcessors += [ TrackingVolumeDisplayer ]

# load the TTree writer
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc() 
# ----------------------
ServiceMgr.THistSvc.Output = [ "val DATAFILE='TrackingGeometryTest.root' TYPE='ROOT' OPT='RECREATE'" ]
from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__GeometryTTreeDumper
GeometryTTreeDumper = Trk__GeometryTTreeDumper(name= 'GeometryTTreeDumper')
ToolSvc += GeometryTTreeDumper
GeometryProcessors += [ GeometryTTreeDumper ]


# ----------------------
from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__GeometryAsciiDumper
GeometryAsciiDumper = Trk__GeometryAsciiDumper(name= 'GeometryAsciiDumper')
ToolSvc += GeometryAsciiDumper
GeometryProcessors += [ GeometryAsciiDumper ]


# Add top algorithms to be run
from TrkDetDescrUnitTests.TrkDetDescrUnitTestsConf import Trk__TrackingGeometryTest
TrackingGeometryTest = Trk__TrackingGeometryTest('TrackingGeometryTest')
TrackingGeometryTest.TrackingGeometrySvc         = AtlasTrackingGeometrySvc
TrackingGeometryTest.TrackingGeometryProcessors  = GeometryProcessors
job += TrackingGeometryTest   # 1 alg, named "TrackingGeometryTest"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

# Output level for HelloAlg only (note name: instance, not type)
TrackingGeometryTest = INFO

# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 10

from AthenaCommon.AppMgr import ServiceMgr
# output level
ServiceMgr.MessageSvc.OutputLevel  = INFO
# increase the number of letter reserved to the alg/tool name from 18 to 30
ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages


#==============================================================
#
# End of job options file
#
###############################################################
include("InDetSLHC_Example/postInclude.SLHC_Setup.py")
