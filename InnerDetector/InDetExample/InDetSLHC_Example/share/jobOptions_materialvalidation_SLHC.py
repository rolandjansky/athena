###############################################################
#
# Job options file for Algorithm MaterialValidation
#
#==============================================================

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
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
# --- printout
DetFlags.Print()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version      = ''

if SLHC_Flags.SLHC_Version() != '' :
 DetVerName =  SLHC_Flags.SLHC_Version()
else :
 DetVerName = jobproperties.Global.DetDescrVersion()


from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.SLHC_Geometry                   = True
TrkDetFlags.PixelBuildingOutputLevel        = VERBOSE
TrkDetFlags.SCT_BuildingOutputLevel         = VERBOSE
TrkDetFlags.TRT_BuildingOutputLevel         = VERBOSE
TrkDetFlags.MagneticFieldCallbackEnforced   = False
TrkDetFlags.TRT_BuildStrawLayers            = False
TrkDetFlags.MaterialFromCool                = True
TrkDetFlags.MaterialDatabaseLocal           = True and TrkDetFlags.MaterialFromCool()
TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
TrkDetFlags.MaterialTagBase                 = 'SLHC_LayerMat_v'
TrkDetFlags.MaterialVersion                 = 6
TrkDetFlags.MaterialMagicTag                = DetVerName

if TrkDetFlags.MaterialDatabaseLocal() is True :
   TrkDetFlags.MaterialDatabaseLocalPath    = ''
   TrkDetFlags.MaterialDatabaseLocalName    = 'SLHC_LayerMaterial-'+DetVerName+'.db'
TrkDetFlags.MagneticFieldCallbackEnforced         = False
TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False

TrkDetFlags.MagneticFieldMode            = 0
TrkDetFlags.MagneticFieldSynchronisation = True 


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

#from AthenaCommon.GlobalFlags import jobproperties
#jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("OFLCOND-SDR-BS14T-ATLAS-00")
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-GEO-06-00-00')

# the global flags    
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

doReadFromCOOL   = True
doFatrasGeometry = False
JobTrackingGeometrySvc = 0

# the bfield 
try:
   import MagFieldServices.SetupField
except:
   include( 'BFieldAth/BFieldAth_jobOptions.py')
   
########################################################################################################
# Event Selector and Material for the Geometry - from COOL / from file
theApp.ExtSvc += [ 'McEventSelector/EventSelector' ]

if doReadFromCOOL and not doFatrasGeometry:
   include ('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
   JobTrackingGeometrySvc = AtlasTrackingGeometrySvc
   AtlasTrackingGeometrySvc.OutputLevel = 1
elif doReadFromCOOL and doFatrasGeometry :  
   include ('FatrasDetDescrSvc/FatrasTrackingGeometrySvc.py')
   JobTrackingGeometrySvc = FatrasTrackingGeometrySvc

#--------------------------------------------------------------
# Events and MessageSvc
#--------------------------------------------------------------

# Number of events to be processed
theApp.EvtMax = 50000
#theApp.EvtMax = 50

ServiceMgr.MessageSvc.OutputLevel             = INFO
ServiceMgr.MessageSvc.defaultLimit            = 10000
ServiceMgr.MessageSvc.Format                  = "% F%37W%S%7W%R%T %0W%M"

################################################################
#  Extrapolation setup
#
# Navigator with Atlasnstruction geometry
from TrkExTools.TrkExToolsConf import Trk__Navigator
ValidationNavigator = Trk__Navigator(name ='ValidationNavigator')
ValidationNavigator.ValidationMode       = True
ValidationNavigator.TrackingGeometrySvc  = JobTrackingGeometrySvc
ToolSvc += ValidationNavigator

# the material mapper
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__MaterialMapper
ValidationMaterialMapper    = Trk__MaterialMapper(name ='MaterialMapper')
ValidationMaterialMapper.TrackingVolumeValidation  = True
ValidationMaterialMapper.DetailedLayerValidation   = True
ValidationMaterialMapper.DetailedSurfaceValidation = False
ToolSvc += ValidationMaterialMapper

# The standard material effects updator
from TrkExTools.TrkExToolsConf import Trk__DummyMaterialEffectsUpdator
ValidationMaterialUpdator                   = Trk__DummyMaterialEffectsUpdator(name='ValidationMaterialUpdator')
ValidationMaterialUpdator.ReferenceMaterial           = False
ValidationMaterialUpdator.ValidationMode              = True
ValidationMaterialUpdator.ValidationMaterialMapper    = ValidationMaterialMapper
ToolSvc += ValidationMaterialUpdator

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()

# the output file
OutputFilePath = ''
OutputFileName = OutputFilePath+'MaterialValidation-'
if doFatrasGeometry :
   OutputFileName = OutputFilePath+'FatrasMaterialValidation-'
ServiceMgr.THistSvc.Output = [ "val DATAFILE='"+OutputFileName+DetVerName+".root' TYPE='ROOT' OPT='RECREATE'"] 

# The standard material effects updator
from TrkExSlPropagator.TrkExSlPropagatorConf import Trk__StraightLinePropagator
ValidationPropagator = Trk__StraightLinePropagator(name='ValidationPropagator')
ToolSvc += ValidationPropagator

# Extrapolator 
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
ValidationExtrapolator         = Trk__Extrapolator(name='ValidationExtrapolator')
ValidationExtrapolator.MaterialEffectsUpdators =  [ ValidationMaterialUpdator ]
ValidationExtrapolator.Navigator               =  ValidationNavigator
ValidationExtrapolator.Propagators             =  [ ValidationPropagator ]
ValidationExtrapolator.StopWithNavigationBreak         = True
ValidationExtrapolator.DetailedNavigationOutput        = True
ValidationExtrapolator.OutputLevel                     = INFO
ToolSvc += ValidationExtrapolator

###############################################################
from TrkDetDescrAlgs.TrkDetDescrAlgsConf import Trk__MaterialValidation

MaterialValidation = Trk__MaterialValidation(name='MaterialValidation')
MaterialValidation.Extrapolator              = ValidationExtrapolator
MaterialValidation.StartPerigeeSigmaD0       = 0.
MaterialValidation.StartPerigeeSigmaZ0       = 0.
MaterialValidation.StartPerigeeMinEta        = -6.0
MaterialValidation.StartPerigeeMaxEta        =  6.0
 
#################################################################

topSequence += MaterialValidation


theAuditorSvc = ServiceMgr.AuditorSvc
#theAuditorSvc.Auditors += [ 'ChronoAuditor']
ServiceMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 5
#theAuditorSvc.Auditors += [ 'MemStatAuditor' ]
MemStatAuditor         = theAuditorSvc.auditor( 'MemStatAuditor' )

# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

