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

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence() 

execfile("ID_only.py")
execfile("geometry.py")

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
# --- input is pool for SLHC 
globalflags.InputFormat = 'pool'

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.SLHC_Geometry 	            = True
TrkDetFlags.PixelBuildingOutputLevel 	    = VERBOSE
TrkDetFlags.SCT_BuildingOutputLevel  	    = VERBOSE
TrkDetFlags.TRT_BuildingOutputLevel  	    = VERBOSE
TrkDetFlags.MagneticFieldCallbackEnforced   = False
TrkDetFlags.TRT_BuildStrawLayers            = False
# TrkDetFlags.MaterialFromCool                = False 
TrkDetFlags.MaterialDatabaseLocal           = False
#  SLHC_Flags.SLHC_Version() != '' and TrkDetFlags.MaterialFromCool()
TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
TrkDetFlags.MaterialTagBase                 = 'SLHC_LayerMat_v'
TrkDetFlags.MaterialVersion                 = 6
TrkDetFlags.MaterialMagicTag                = jobproperties.Global.DetDescrVersion()
if TrkDetFlags.MaterialDatabaseLocal() is True :
   TrkDetFlags.MaterialMagicTag                = SLHC_Flags.SLHC_Version()
   TrkDetFlags.MaterialDatabaseLocalPath    = ''
   TrkDetFlags.MaterialDatabaseLocalName    = 'SLHC_LayerMaterial-'+SLHC_Flags.SLHC_Version()+'.db'
TrkDetFlags.MagneticFieldCallbackEnforced         = False
# TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
# TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False


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
MessageSvc.defaultLimit            = 500
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
# Nigel: copied the file locally since it wasn't found otherwise.
tempVersion = SLHC_Flags.SLHC_Version.get_Value()
SLHC_Flags.SLHC_Version = "21-22-23"
#execfile("AtlasTrackingGeometrySvc.py")
include ('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
SLHC_Flags.SLHC_Version = tempVersion
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

