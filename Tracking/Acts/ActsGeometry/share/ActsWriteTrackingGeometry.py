# Use Global flags and DetFlags.
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

# Just the pixel and SCT
DetFlags.ID_setOn()
DetFlags.detdescr.pixel_setOn()
DetFlags.detdescr.SCT_setOn()

# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

# Select the geometry version. 
globalflags.DetDescrVersion = 'ATLAS-R2-2016-00-00-00'

# LorentzAngle Svc needs field now
import MagFieldServices.SetupField

# THIS ACTUALLY DOES STUFF!!
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# For misalignments
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
conddb.addOverride("/Indet/Align", "InDetAlign_R2_Nominal")

from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc( OutputLevel=INFO )
from ActsGeometry import ActsGeometryConf
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condSeq += ActsGeometryConf.NominalAlignmentCondAlg("NominalAlignmentCondAlg",
                                                     OutputLevel=VERBOSE)

from AthenaCommon.AppMgr import ServiceMgr
from ActsGeometry.ActsGeometryConfig import ActsTrackingGeometrySvc
ServiceMgr = ActsTrackingGeometrySvc()

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from ActsGeometry.ActsGeometryConf import ActsWriteTrackingGeometry
alg = ActsWriteTrackingGeometry(OutputLevel = VERBOSE)

trkGeomTool = CfgMgr.ActsTrackingGeometryTool("ActsTrackingGeometryTool")
trkGeomTool.OutputLevel = INFO;
alg.TrackingGeometryTool = trkGeomTool

alg.ObjWriterTool.OutputDirectory = "obj"
alg.ObjWriterTool.SubDetectors = [
    "Pixel", 
    "SCT",
    "TRT",
]


job += alg

theApp.EvtMax = 1

