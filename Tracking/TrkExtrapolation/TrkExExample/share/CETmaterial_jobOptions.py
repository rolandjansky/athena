###############################################################
#
# Job options file fro the ExtrapolatorTest Algorithm
#
#==============================================================
#theApp.setup( MONTECARLO )
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

# Global Flags to be set ######################################
varInit=dir()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence() 

# some very general flags, EvtMax, SkipEvents, PoolESDInput etc...
include ("AthenaCommon/AthenaCommonFlags.py")
# prevent this file to be included a second time
include.block("AthenaCommon/AthenaCommonFlags.py")

if 'GlobalFlags' in varInit:
   print "GlobalFlags already imported:"
else:
   from AthenaCommon.GlobalFlags import GlobalFlags

GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetGeo._beenSet = True
#GlobalFlags.DetGeo._flag_ctbh8 = True

GlobalFlags.DataSource._beenSet     = True
GlobalFlags.DataSource._flag_geant4 = True

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()

TrkCaloGeo = False

###############################################################
# Layout
DetDescrVersion = "ATLAS-CSC-02-00-00"

# needed for condDB setup
# TC: condDB is configured by GeoModel
#include( "IOVDbSvc/IOVRecExCommon.py" )

include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )

################################################################

# Number of events to be processed
theApp.EvtMax = 1

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel             =  3
MessageSvc.defaultLimit            = 1000000
MessageSvc.Format                  = "% F%37W%S%7W%R%T %0W%M"

################################################################
# load the AtlasExtrapolator
include("TrkExExample/CombinedExtrapolator.py")

ToolSvc.MuonTrackingGeometryBuilder.BuildActiveMaterial = True
ToolSvc.MuonTrackingGeometryBuilder.BuildInertMaterial = True
#ToolSvc.MuonTrackingGeometryBuilder.BlendInertMaterial = False
ToolSvc.MuonTrackingGeometryBuilder.StaticPartition3D = True
ToolSvc.MuonTrackingGeometryBuilder.AdjustStatic = True
#ToolSvc.MuonTrackingGeometryBuilder.EtaBarrelPartitions = 1
#ToolSvc.MuonTrackingGeometryBuilder.EtaInnerEndcapPartitions = 1
#ToolSvc.MuonTrackingGeometryBuilder.EtaOuterEndcapPartitions = 1
#ToolSvc.MuonTrackingGeometryBuilder.PhiPartitions = 1

ToolSvc.MuonInertMaterialBuilder.OutputLevel = VERBOSE
ToolSvc.MuonInertMaterialBuilder.SimplifyGeometry = True
ToolSvc.MuonInertMaterialBuilder.SimplifyGeometryToLayers = False
#ToolSvc.MuonInertMaterialBuilder.BuildRails = 1

ToolSvc.CombinedExtrapolator.OutputLevel = VERBOSE

ToolSvc.AtlasSTEP_Propagator.MultipleScattering = True

from TrkExAlgs.TrkExAlgsConf import Trk__CETmaterial
CETmaterial = Trk__CETmaterial('CETmaterial')

CETmaterial.Extrapolator = CombinedExtrapolator
CETmaterial.NumberOfScanTracks = 360
CETmaterial.StartPerigeeMinZ0  = 0.
CETmaterial.StartPerigeeMaxZ0  = 0.
CETmaterial.StartPerigeeMinTheta  = 75.*deg
CETmaterial.StartPerigeeMaxTheta  = 75.*deg
#CETmaterial.StartPerigeeMinPhi  = 45.*deg
#CETmaterial.StartPerigeeMaxPhi  = 135*deg
#CETmaterial.StartPerigeeMinP  = 10.*GeV
#CETmaterial.StartPerigeeMaxP  =  10.*GeV
CETmaterial.CheckActiveLayers = False
CETmaterial.PrintMaterial = True
CETmaterial.PrintActivePos = False

topSequence += CETmaterial

#from VP1Algs.VP1AlgsConf import VP1Alg
#topSequence += VP1Alg()
