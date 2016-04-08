#--------------------------------------------------------------
# Job configuration
#--------------------------------------------------------------

from MinBiasD3PDMaker.MinBiasD3PDMakerFlags import minbiasD3PDflags
from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
ServiceMgr.MessageSvc.defaultLimit  = 9999999

DoReconstruction = False # True # 
DoTruth = True # False # 
DoTracklets = True # False # 
DoTrigger = True # False # 
DoBeamSpot = True
DoUnassociatedHits = True
DoV0s = True

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

## Global flags
DetDescrVersion = 'ATLAS-GEO-10-00-00'

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
globalflags.DetGeo = 'atlas'
globalflags.InputFormat = 'pool'
globalflags.DetDescrVersion = DetDescrVersion
globalflags.DataSource  = 'geant4' # 'data' # 
jobproperties.Beam.beamType = 'collisions' # 'cosmics' # 'singlebeam' # 
if globalflags.DataSource == 'geant4':
   globalflags.ConditionsTag = 'OFLCOND-DR-BS900-ANom-03'
   
globalflags.print_JobProperties()

## Configure conditions
if len(globalflags.ConditionsTag())!=0:
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag()) 

## Configure Pixel conditions
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/PIXEL/PixMapOverlay', 'PixMapOverlay-Coll09-75-00')

## Beam properties
import AthenaCommon.SystemOfUnits as Units
jobproperties.Beam.numberOfCollisions = 2.0 # default in RecExCommon for 900 GeV
jobproperties.Beam.energy             = 450.*Units.GeV
jobproperties.Beam.beamType           = 'collisions'

## Solenoid state
from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.solenoidOn=True

## Load Tracking Tools
include( "MinBiasD3PDMaker/RecoOptions.py" )

## Configure Trigger reading
if DoTrigger and False:
   from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
   cfg = TriggerConfigGetter("ReadPool")

## Configure Beam spot service
if globalflags.DataSource == 'data':
   minbiasD3PDflags.BSDBFolderName = 'IndetBeampos-r988-Collision_2009_02-v0'

#--------------------------------------------------------------
# Configure the MinBiasD3PDMaker
#--------------------------------------------------------------

## Configure input
ServiceMgr.EventSelector.InputCollections = [ "ESD.pool.root" ]
theApp.EvtMax = 10
ServiceMgr.EventSelector.SkipEvents = 0

# output
minbiasD3PDflags.FileName = '/tmp/zaidan/MinBiasD3PD.root'

# TrackD3PDMaker Flags
from TrackD3PDMaker.TrackD3PDMakerFlags import *
TrackD3PDFlags.doTruth = DoTruth
TrackD3PDFlags.storeDiagonalCovarianceAsErrors = True
TrackD3PDFlags.storeHitTruthMatching = True
TrackD3PDFlags.storeDetailedTruth = True
TrackD3PDFlags.storePixelHitsOnTrack = True
TrackD3PDFlags.storeSCTHitsOnTrack = True
TrackD3PDFlags.storeTRTHitsOnTrack = True
TrackD3PDFlags.storePixelOutliersOnTrack = True
TrackD3PDFlags.storeSCTOutliersOnTrack = True
TrackD3PDFlags.storeTRTOutliersOnTrack = True
TrackD3PDFlags.storePixelHolesOnTrack = True
TrackD3PDFlags.storeSCTHolesOnTrack = True
TrackD3PDFlags.storeTRTHolesOnTrack = True
TrackD3PDFlags.storeTrackPredictionAtBLayer = True
TrackD3PDFlags.storeDetailedTrackPredictionAtBLayer = True
TrackD3PDFlags.storeTrackInfo = True
TrackD3PDFlags.storeTrackFitQuality = True
TrackD3PDFlags.storeTrackSummary = True
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 3
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 3
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails = 3
TrackD3PDFlags.storeTrackMomentum = True
TrackD3PDFlags.vertexPositionLevelOfDetails = 3
TrackD3PDFlags.storeVertexFitQuality = True
TrackD3PDFlags.storeVertexKinematics = True
TrackD3PDFlags.storeVertexPurity = True
TrackD3PDFlags.storeVertexTrackAssociation = True
TrackD3PDFlags.storeVertexTrackIndexAssociation = True

include("MinBiasD3PDMaker/MinBiasD3PD_jobOptions.py")
