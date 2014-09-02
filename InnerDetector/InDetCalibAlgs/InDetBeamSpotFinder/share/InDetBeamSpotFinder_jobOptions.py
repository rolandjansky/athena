


# Input files
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = ["aod.pool.root"]


# Number of events to be processed (default is 10)
theApp.EvtMax = -1 

ToolSvc = Service('ToolSvc')

# Select Vertex or Track Algorithms
useVertex = True 
# Do Histogramming
doHists   = False 

if useVertex:
    from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotVertex
    InDetBeamSpotVertex = InDet__InDetBeamSpotVertex(OutputLevel    = INFO,
                                                     DoHists        = doHists,
                                                     UseLikelihood  = True
                                                     )
    ToolSvc +=  InDetBeamSpotVertex
else:
    from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotTrackChi2
    InDetBeamSpotTrackChi2 = InDet__InDetBeamSpotTrackChi2(OutputLevel = INFO)
    ToolSvc +=  InDetBeamSpotTrackChi2
    # InDetBeamSpotTrackChi2.nTracks_0 = 500
    # InDetBeamSpotTrackChi2.nTracks_1 = 1500
    # InDetBeamSpotTrackChi2.nTracks = InDetBeamSpotTrackChi2.nTracks_0 + InDetBeamSpotTrackChi2.nTracks_1
    # Use default or TrackParticleCandidate for AOD
    # InDetBeamSpotTrackChi2.TrackContainer = "TrackParticleCandidate"

# For output to a Database
from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotDbWriterTool
InDetBeamSpotDbWriterTool = InDet__InDetBeamSpotDbWriterTool(OutputLevel = INFO,
                                                             Tag = "nominal")
ToolSvc += InDetBeamSpotDbWriterTool

# Main Algorithm
from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotFinder as InDetBeamSpotFinder
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += InDetBeamSpotFinder()
InDetBeamSpotFinder = InDetBeamSpotFinder(OutputLevel = INFO,
                                          DoHists     = doHists,
                                          WriteDb     = True)
if useVertex:
    InDetBeamSpotFinder.BeamSpotTool = InDetBeamSpotVertex
else:
    InDetBeamSpotFinder.BeamSpotTool = InDetBeamSpotTrackChi2

# Call the histogramming service
if doHists:
  # produce ROOT ntuple using THistSvc
  from AthenaCommon.AppMgr import ServiceMgr
  if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
  ServiceMgr.THistSvc.Output+=[ "INDETBEAMSPOTFINDER DATAFILE='beamspot.root' OPT='RECREATE'"]

# Write database to a sqlite file
from AthenaCommon.AppMgr    import ToolSvc
from AthenaCommon.AppMgr    import ServiceMgr as svcMgr
from IOVDbSvc.IOVDbSvcConf  import IOVDbSvc
svcMgr += IOVDbSvc()
#IOVDbSvc.dbConnection  = "impl=cool;techno=sqlite;schema=beamspot.db;X:TESTCOOL"
IOVDbSvc.dbConnection  = "sqlite://;schema=beamspot.db;dbname=BEAMSPOT"

    

