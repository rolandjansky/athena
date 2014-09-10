#--------------------------------------------------------------
# Job configuration
#--------------------------------------------------------------

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

if not DoReconstruction:
   DoTracklets = False
     
#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

## Global flags
#DetDescrVersion = 'ATLAS-GEO-08-00-02'

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
globalflags.DetGeo = 'atlas'
globalflags.InputFormat = 'pool'
globalflags.DetDescrVersion = DetDescrVersion
globalflags.DataSource  = 'geant4' # 'data' # 
jobproperties.Beam.beamType = 'collisions' # 'cosmics' # 'singlebeam' # 
if globalflags.DataSource != 'geant4':
   DoTruth = False
else:
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
include( "TrackD3PDMaker/RecoOptions.py" )

## Configure Trigger reading
if DoTrigger and False:
   from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
   cfg = TriggerConfigGetter("ReadPool")

## Configure Beam spot service
if globalflags.DataSource == 'data':
    include("InDetBeamSpotService/BeamCondSvc.py")
    conddb.addOverride("/Indet/Beampos", 'IndetBeampos-r988-Collision_2009_02-v0')

#--------------------------------------------------------------
# Configure the TrackD3PDMaker
#--------------------------------------------------------------

import MinBiasD3PDMaker
import TrackD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

## Output file name
outFileName = 'TrackD3PD.root'

## Top Sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## Event Info
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
## Truth Tracks
from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
## Truth Vertices
from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
## Reco Tracks
from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
from TrackD3PDMaker.TrackD3PDObject import PixelTrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import SCTTrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import TRTTrackD3PDObject
## Reco Vertex
from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
from TrackD3PDMaker.V0D3PDObject import V0D3PDObject
## CTP RDO
from TrigMbD3PDMaker.CtpRdoD3PDObject import CtpRdoD3PDObject

## D3PD Maker alg
alg = D3PDMakerCoreComps.MakerAlg('MinBiasTree',
                                  topSequence,
                                  outFileName)

## Add blocks to the tree
alg += EventInfoD3PDObject(0)
alg += V0D3PDObject(10)
if DoTrigger:
   alg += CtpRdoD3PDObject(10)
if DoTruth:
   alg += TruthTrackD3PDObject(10)
   alg += TruthVertexD3PDObject(10)
   alg += TrackParticleD3PDObject(10)
   alg += PrimaryVertexD3PDObject(10)
   if DoTracklets:
      alg += PixelTrackD3PDObject(10)
      alg += SCTTrackD3PDObject(10)
      alg += TRTTrackD3PDObject(10)
else:
   alg += TrackParticleD3PDObject(8)
   alg += PrimaryVertexD3PDObject(8)
   if DoTracklets:
      alg += PixelTrackD3PDObject(8)
      alg += SCTTrackD3PDObject(8)
      alg += TRTTrackD3PDObject(8)

## Configure input
ServiceMgr.EventSelector.InputCollections = [ "ESD.pool.root" ]
theApp.EvtMax = -1
ServiceMgr.EventSelector.SkipEvents = 0
