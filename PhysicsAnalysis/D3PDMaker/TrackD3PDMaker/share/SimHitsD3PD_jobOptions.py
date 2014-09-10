#--------------------------------------------------------------
# Job configuration
#--------------------------------------------------------------

from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc

# configure MsgSvc
ServiceMgr.MessageSvc.defaultLimit  = 9999999
ServiceMgr.MessageSvc.OutputLevel = INFO

#--------------------------------------------------------------
## Configure input
#--------------------------------------------------------------
ServiceMgr.EventSelector.InputCollections = [ "Hits.pool.root" ]
theApp.EvtMax = -1
ServiceMgr.EventSelector.SkipEvents = 0

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

## Global flags
DetDescrVersion = 'ATLAS-GEO-20-00-01'

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
   globalflags.ConditionsTag = 'OFLCOND-MC12-SIM-00'
   
globalflags.print_JobProperties()

## Configure conditions
if len(globalflags.ConditionsTag())!=0:
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag()) 

# setup detector description
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()

# load det description
include('RecExCond/AllDet_detDescr.py')

#--------------------------------------------------------------
# Configure the TrackD3PDMaker
#--------------------------------------------------------------

## Output file name
outFileName = 'SimHitsD3PD.root'

## Top Sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## Event Info
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
## Truth Tracks
from TrackD3PDMaker.SiHitD3PDObject import PixelSiHitD3PDObject
from TrackD3PDMaker.SiHitD3PDObject import SctSiHitD3PDObject
from TrackD3PDMaker.TRTUncompressedHitD3PDObject import TRTUncompressedHitD3PDObject

## D3PD Maker alg
import D3PDMakerCoreComps
alg = D3PDMakerCoreComps.MakerAlg('SimHits', topSequence, outFileName)

## Add blocks to the tree
alg += EventInfoD3PDObject(0)
alg += PixelSiHitD3PDObject(10)
alg += SctSiHitD3PDObject(10)
alg += TRTUncompressedHitD3PDObject(10)
