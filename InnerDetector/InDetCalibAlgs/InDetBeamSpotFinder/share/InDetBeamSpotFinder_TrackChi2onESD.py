## Data type (ESD, AOD)
datatype = "ESD"

## magnetic field
solenoid = "on"
toroid = "off"

## get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

AthenaEventLoopMgr = Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = INFO



# Input files
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141749/data09_900GeV.00141749.physics_MinBias.merge.DESD_COLLCAND.f187_m305/data09_900GeV.00141749.physics_MinBias.merge.DESD_COLLCAND.f187_m305._lb0023-lb0042.1"]
ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141749/data09_900GeV.00141749.physics_MinBias.merge.DESD_COLLCAND.f187_m305/data09_900GeV.00141749.physics_MinBias.merge.DESD_COLLCAND.f187_m305._lb0043-lb0062.1"]
ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141749/data09_900GeV.00141749.physics_MinBias.merge.DESD_COLLCAND.f187_m305/data09_900GeV.00141749.physics_MinBias.merge.DESD_COLLCAND.f187_m305._lb0063-lb0082.1"]
ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141749/data09_900GeV.00141749.physics_MinBias.merge.DESD_COLLCAND.f187_m305/data09_900GeV.00141749.physics_MinBias.merge.DESD_COLLCAND.f187_m305._lb0083-lb0101.1"]
ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141811/data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.f187_m305/data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.f187_m305._lb0127-lb0133.1"]
ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141811/data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.f187_m305/data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.f187_m305._lb0134-lb0141.1"]
ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141811/data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.f187_m305/data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.f187_m305._lb0142-lb0150.1"]
ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141811/data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.f187_m305/data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.f187_m305._lb0151-lb0159.1"]
ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data09_900GeV/physics_MinBias/0141811/data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.f187_m305/data09_900GeV.00141811.physics_MinBias.merge.DESD_COLLCAND.f187_m305._lb0160-lb0165.1"]

# Number of events to be processed (default is 10)
theApp.EvtMax = -1 


##

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.BFieldFlags import jobproperties
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'

# globalflags.DetDescrVersion = "ATLAS-GEO-03-00-00"
globalflags.DetDescrVersion = "ATLAS-GEO-08-00-02"


from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.all_setOn()
import AtlasGeoModel.GeoModelInit
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


# set the conditions db tag
IOVDbSvc = Service("IOVDbSvc")
#IOVDbSvc.GlobalTag="COMCOND-REPC-001-03"
IOVDbSvc.GlobalTag="COMCOND-ES1CT-003-00"
jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
if solenoid == "on":
    jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
if toroid == "on":
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
if solenoid == "off":
    jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
if toroid == "off":
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(False)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(False)


    
include("RecExCond/AllDet_detDescr.py")

PoolSvc=Service("PoolSvc")
PoolSvc.SortReplicas=True
PoolSvc.ReadCatalog+= [ "prfile:poolcond/PoolFileCatalog.xml" ]
DBReplicaSvc=Service("DBReplicaSvc")
DBReplicaSvc.UseCOOLSQLite=False




ToolSvc = Service('ToolSvc')

# Needed to use TrackSummaryTool:
include("./ReadInDet_jobOptions.py")


# Setup of the BeamSpotFinder:
# Select Vertex or Track Algorithms
useVertex = False 
# Do Histogramming
doHists   = True 

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
    InDetBeamSpotTrackChi2.OutputLevel   = INFO
    InDetBeamSpotTrackChi2.nTracks_0     = 500
    InDetBeamSpotTrackChi2.nTracks_1     = 3000
    InDetBeamSpotTrackChi2.nTracks       = InDetBeamSpotTrackChi2.nTracks_0 + InDetBeamSpotTrackChi2.nTracks_1
    InDetBeamSpotTrackChi2.ErrorFactor_0 = 1000.
    InDetBeamSpotTrackChi2.ErrorFactor_1 = 10.
    InDetBeamSpotTrackChi2.PtCut         = 500.0
    InDetBeamSpotTrackChi2.D0Match_0_Cut = 100.0
    InDetBeamSpotTrackChi2.D0Match_1_Cut = 10.0
    InDetBeamSpotTrackChi2.D0Match_2_Cut = 3.0
    InDetBeamSpotTrackChi2.BL_X_seed     = 0.0
    InDetBeamSpotTrackChi2.BL_a_seed     = 0.0
    InDetBeamSpotTrackChi2.BL_Y_seed     = 0.0
    InDetBeamSpotTrackChi2.BL_b_seed     = 0.0
    InDetBeamSpotTrackChi2.nBLayer_Q_cut    = 1
    InDetBeamSpotTrackChi2.PixShared_Q_cut  = 0
    InDetBeamSpotTrackChi2.PixHoles_Q_cut   = 1
    InDetBeamSpotTrackChi2.ChiOvDof_Q_cut   = 3.
        
    # Use default or TrackParticleCandidate for AOD
    # InDetBeamSpotTrackChi2.TrackContainer = "TrackParticleCandidate"
    InDetBeamSpotTrackChi2.TrackContainer = "Tracks"

    print InDetBeamSpotTrackChi2


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
InDetBeamSpotFinder.LumiRange=5
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

    

