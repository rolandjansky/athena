###############################################################
#
# Loads Detector Description for Pixel and SCT
# and prints out various quanities for each detector element.
#
###############################################################


# Use Global flags and DetFlags.
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
ServiceMgr.MessageSvc.defaultLimit = 20000

# Just the pixel and SCT
DetFlags.ID_setOn()
DetFlags.detdescr.pixel_setOn()
DetFlags.detdescr.SCT_setOn()
DetFlags.Calo_setOff()

# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

# Select the geometry version.
globalflags.DetDescrVersion = 'ATLAS-R2-2016-00-00-00'

# print "HERE"
# print globalflags.DetDescrVersion

# LorentzAngle Svc needs field now
import MagFieldServices.SetupField

# Initialize geometry
# THIS ACTUALLY DOES STUFF!!
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# For misalignments
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
# conddb.addOverride("/Indet/Align", "InDetAlign_R2_Nominal")

from AthenaCommon.AppMgr import ServiceMgr

# Read material step file
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections =  ["MaterialStepFile.root"]


from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.OutputLevel( INFO )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.EnableConditions( True )
# AlgScheduler.setDataLoaderAlg( "SGInputLoader" )


from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc( OutputLevel=INFO )

# ServiceMgr += CfgMgr.THistSvc()
# ServiceMgr.THistSvc.Output += ["MATTRACKVAL DATAFILE='MaterialTracks.root' OPT='RECREATE'"]
# ServiceMgr.ToolSvc.OutputLevel = VERBOSE


# Set up ACTS tracking geometry service
from ActsGeometry.ActsGeometryConf import ActsTrackingGeometrySvc
trkGeomSvc = ActsTrackingGeometrySvc()
trkGeomSvc.OutputLevel = INFO
trkGeomSvc.BarrelMaterialBins = [40, 60] # phi z
trkGeomSvc.EndcapMaterialBins = [50, 20] # phi r
trkGeomSvc.BuildSubDetectors = [
  "Pixel",
  "SCT",
  # "TRT",
  # "Calo",
]
ServiceMgr += trkGeomSvc

trkGeomTool = CfgMgr.ActsTrackingGeometryTool("ActsTrackingGeometryTool")
trkGeomTool.OutputLevel = INFO;

# Set up ACTS extrapolation cell writer service
# exCellWriterSvc = CfgMgr.ActsExCellWriterSvc("ActsExCellWriterSvc")
# exCellWriterSvc.FilePath = "excells_charged_mapping.root"
# ServiceMgr += exCellWriterSvc
mTrackWriterSvc = CfgMgr.ActsMaterialTrackWriterSvc("ActsMaterialTrackWriterSvc")
mTrackWriterSvc.OutputLevel = INFO
mTrackWriterSvc.FilePath = "MaterialTracks_mapping.root"
# mTrackWriterSvc.MaxQueueSize = 10
ServiceMgr += mTrackWriterSvc

mMaterialStepConverterTool = CfgMgr.ActsMaterialStepConverterTool("ActsMaterialStepConverterTool")
mMaterialStepConverterTool.OutputLevel = INFO

mActsSurfaceMappingTool = CfgMgr.ActsSurfaceMappingTool("ActsSurfaceMappingTool")
mActsSurfaceMappingTool.OutputLevel = INFO
mActsSurfaceMappingTool.TrackingGeometryTool = trkGeomTool

mActsMaterialJsonWriterTool = CfgMgr.ActsMaterialJsonWriterTool("ActsMaterialJsonWriterTool")
mActsMaterialJsonWriterTool.OutputLevel = VERBOSE
mActsMaterialJsonWriterTool.FilePath = "material-maps.json"

from ActsGeometry import ActsGeometryConf

## SET UP ALIGNMENT CONDITIONS ALGORITHM
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condSeq += ActsGeometryConf.NominalAlignmentCondAlg("NominalAlignmentCondAlg",
                                                 OutputLevel=INFO)
## END OF CONDITIONS SETUP

# Set up algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

eventPrintFrequency = 1000

if hasattr(ServiceMgr,"AthenaEventLoopMgr"):
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = eventPrintFrequency
if hasattr(ServiceMgr,"AthenaHiveEventLoopMgr"):
    ServiceMgr.AthenaHiveEventLoopMgr.EventPrintoutInterval = eventPrintFrequency

from GaudiAlg.GaudiAlgConf import EventCounter
job += EventCounter(Frequency=1000)

# Set up material mapping algorithm
from ActsGeometry.ActsGeometryConf import ActsMaterialMapping

alg = ActsMaterialMapping()
alg.Cardinality = 0#nThreads
alg.MaterialStepConverterTool = mMaterialStepConverterTool
alg.SurfaceMappingTool = mActsSurfaceMappingTool
alg.MaterialJsonWriterTool = mActsMaterialJsonWriterTool
alg.OutputLevel = INFO
job += alg
