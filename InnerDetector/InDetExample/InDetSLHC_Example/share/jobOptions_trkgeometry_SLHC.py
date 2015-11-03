#############################################################
# Run jobOptions for the Algorithm: TrackingGeometryTest
#
# This Algorithm is a debug algorithm of the TrackingGeometry,
# it retrieves the specified instance of the TrackingGeometry
# from the DetectorStore and outputs the contents in a Ntuple
#
#############################################################
#
# author: Andreas.Salzburger@cern.ch
#
#############################################################
#
# date: 13/01/2005
#
#############################################################

# set to monte carlo ---------------------------
import AthenaCommon.AtlasUnixStandardJob    
from AthenaCommon import AthenaCommonFlags
from AthenaCommon.AppMgr import theApp    
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence() 

# the global detflags
from AthenaCommon.DetFlags import DetFlags
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- no TRT for SLHC
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOff()
DetFlags.makeRIO.TRT_setOff()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# --- printout
DetFlags.Print()


from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
#SLHC_Flags.SLHC_Version      = 'SLHC-19-20'
SLHC_Flags.SLHC_Version      = ''


from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-SIM-01-00-00")
# Temporary until BField tag for ATLAS-SLHC-01-00-00 put into COOL
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-GEO-06-00-00')


# Global Flags to be set ######################################
#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
# --- input is pool for SLHC 
globalflags.InputFormat = 'pool'

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-SIM-01-00-00")       
# the global flags    
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.SLHC_Geometry 	            = True
TrkDetFlags.PixelBuildingOutputLevel 	    = VERBOSE
TrkDetFlags.SCT_BuildingOutputLevel  	    = VERBOSE
TrkDetFlags.TRT_BuildingOutputLevel  	    = VERBOSE
TrkDetFlags.MagneticFieldCallbackEnforced   = False
TrkDetFlags.TRT_BuildStrawLayers            = False
TrkDetFlags.MaterialFromCool                = True
TrkDetFlags.MaterialDatabaseLocal           =   SLHC_Flags.SLHC_Version() != '' and TrkDetFlags.MaterialFromCool()
TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
TrkDetFlags.MaterialTagBase                 = 'SLHC_LayerMat_v'
TrkDetFlags.MaterialVersion                 = 6
TrkDetFlags.MaterialMagicTag                = jobproperties.Global.DetDescrVersion()
if TrkDetFlags.MaterialDatabaseLocal() is True :
   TrkDetFlags.MaterialMagicTag                = SLHC_Flags.SLHC_Version()
   TrkDetFlags.MaterialDatabaseLocalPath    = ''
   TrkDetFlags.MaterialDatabaseLocalName    = 'SLHC_LayerMaterial-'+SLHC_Flags.SLHC_Version()+'.db'
TrkDetFlags.MagneticFieldCallbackEnforced         = False
TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False


##############################################################
# test the different geometries

GeometryMode = 1

# EmptyGeometry          = 0
# AtlasGeometry          = 1
# FatrasGeometry         = 2
# FatrasGenericGeometry  = 3

#--------------------------------------------------------------
# Events and MessageSvc
#--------------------------------------------------------------

# Number of events to be processed
theApp.EvtMax = 1
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel             = INFO
MessageSvc.defaultLimit            = 10000000
MessageSvc.Format                  = "% F%37W%S%7W%R%T %0W%M"

#--------------------------------------------------------------
# Input of POOL file for LayerMaterial 
#--------------------------------------------------------------

# the bfield
try:
   import MagFieldServices.SetupField
except:
# fall back option 
   include( 'BFieldAth/BFieldAth_jobOptions.py')

# the prepared Tracking geometry svc
include ('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
TrackingGeometrySvc = AtlasTrackingGeometrySvc

#--------------------------------------------------------------
# Assign the TrackingGeometry to the Algorithm
#--------------------------------------------------------------

from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__CylinderVolumeCreator
CylinderVolumeCreator = Trk__CylinderVolumeCreator( name = 'CylinderVolumeCreator') 
CylinderVolumeCreator.OutputLevel = VERBOSE
ToolSvc += CylinderVolumeCreator

# the layer material inspector
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerMaterialInspector
LayerMaterialInspector = Trk__LayerMaterialInspector(name= 'LayerMaterialRecorder')
LayerMaterialInspector.OutputLevel = VERBOSE

from AthenaCommon.AppMgr import ToolSvc 
ToolSvc += LayerMaterialInspector

# the tracking volume displayer
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeDisplayer
TrackingVolumeDisplayer = Trk__TrackingVolumeDisplayer(name= 'TrackingVolumeDisplayer')
TrackingVolumeDisplayer.TrackingVolumeOutputFile = 'TrackingVolumes-'+TrkDetFlags.MaterialMagicTag()+'.C'
TrackingVolumeDisplayer.LayerOutputFile          = 'Layers-'+TrkDetFlags.MaterialMagicTag()+'.C'
TrackingVolumeDisplayer.SurfaceOutputFile        = 'Surfaces-'+TrkDetFlags.MaterialMagicTag()+'.C'
ToolSvc += TrackingVolumeDisplayer


# set up the Geometry Builder test
from TrkDetDescrAlgs.TrkDetDescrAlgsConf import Trk__GeometryBuilderTest
GeometryBuilderTest = Trk__GeometryBuilderTest(name ='GeometryBuilderTest')
GeometryBuilderTest.TrackingGeometrySvc     = TrackingGeometrySvc
GeometryBuilderTest.DisplayTrackingVolumes  = True
GeometryBuilderTest.RecordLayerMaterial     = True
GeometryBuilderTest.LayerMaterialInspector  = LayerMaterialInspector
GeometryBuilderTest.TrackingVolumeDisplayer = TrackingVolumeDisplayer
GeometryBuilderTest.WriteNtuple             = True

#--------------------------------------------------------------
# ROOT Ntuple output
#--------------------------------------------------------------
theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'
# Ntuple

# THist service
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr +=THistSvc()
ServiceMgr.THistSvc.Output = [ "val DATAFILE='TrackingGeometryTest.root' TYPE='ROOT' OPT='RECREATE'" ]

# to switch off un-stamped layers
#GeometryBuilderTest.LayerIndexOutputMax     = 511530510
GeometryBuilderTest.LayerIndexOutputMax     = 1000000000
  
#from StoreGate.StoreGateConf import StoreGateSvc
#EventStore    = StoreGateSvc('StoreGateSvc')
#DetectorStore = StoreGateSvc('DetectorStore')

#GeometryBuilderTest.DetStore = EventStore
#GeometryBuilderTest.EvtStore = DetectorStore

from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerMaterialInspector
LayerMaterialInspector = Trk__LayerMaterialInspector(name= 'LayerMaterialRecorder')
LayerMaterialInspector.OutputLevel = 1

from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeHelper
TrackingVolumeHelper = Trk__TrackingVolumeHelper(name='TrackingVolumeHelper')
TrackingVolumeHelper.OutputLevel = 1


from AthenaCommon.AppMgr import ToolSvc
ToolSvc += LayerMaterialInspector
ToolSvc += TrackingVolumeHelper

GeometryBuilderTest.LayerMaterialInspector = LayerMaterialInspector 
   
topSequence += GeometryBuilderTest

# VP1 setup
from VP1Algs.VP1AlgsConf import VP1Alg
topSequence += VP1Alg()

# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

#--------------------------------------------------------------
# ROOT Ntuple output
#--------------------------------------------------------------
theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'
# Ntuple

# THist service
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr +=THistSvc()
ServiceMgr.THistSvc.Output = [ "val DATAFILE='TrackingGeometryTest.root' TYPE='ROOT' OPT='RECREATE'" ]

