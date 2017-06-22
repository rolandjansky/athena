# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: T2VertexBeamSpotConfig.py 775663 2016-09-28 15:46:56Z masik $
#

from TrigT2BeamSpot.TrigT2BeamSpotConf import PESA__T2VertexBeamSpot
from TrigVertexFitter.TrigVertexFitterConf import TrigPrimaryVertexFitter
from AthenaCommon.AppMgr import ToolSvc

# TrigPrimaryVertexFitter: D. Emeliyanov (author)
PrimaryVertexFitter = TrigPrimaryVertexFitter(zVariance=3.0, CreateTrackLists=True)
ToolSvc += PrimaryVertexFitter

class T2VertexBeamSpot_Fex ( PESA__T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_Fex", detail=1):
        super(T2VertexBeamSpot_Fex, self).__init__(name)

        # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
        from TrigT2BeamSpot.T2VertexBeamSpotMonitoring import T2VertexBeamSpotOnlineMonitoring, T2VertexBeamSpotCosmicMonitoring, T2VertexBeamSpotValidationMonitoring
        validation = T2VertexBeamSpotValidationMonitoring()
        online     = T2VertexBeamSpotOnlineMonitoring(detail=detail)
        cosmic     = T2VertexBeamSpotCosmicMonitoring() 

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 12] # see T2VertexBeamSpotImpl.h: Timers::numTimers

        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
        # Beam Conditions Service
        self.BeamCondSvcName   = "BeamCondSvc" # Name of the service for the BeamCondSvc
        
        # Setup the self algorithm parameters
        # Algorithm IDs: SiTrack = 1, IDScan = 2, TrtXK = 4 
        # (from /offline/Trigger/TrigEvent/TrigInDetEvent/TrigInDetEvent/TrigInDetTrack.h)
        self.SaveAllVertices     = False    # Save histograms for all vertices
        self.WeightClusterZ      = True     # Use the track Z0 weighted cluster Z position as seed            
        self.ReclusterSplit      = False    # Recluster split track collections before vertex fitting
        self.attachVertices      = True     # Write the good vertices to StoreGate
        self.attachSplitVertices = False    # Write split vertices to StoreGate
        self.nSplitVertices      = 2        # Turn on (>1) or off vertex splitting
        self.vertexCollName      = "TrigBeamSpotVertex" # Output vertex collection Name
        
        self.TotalNTrackMin      = 4        # Minimum number of tracks required in an event
        self.TrackMinPt          = 0.5      # Minimum track pT to be considered for vertexing
        self.TrackSeedPt         = 0.7      # Minimum track pT to be considered for seeding a vertex fit
        self.TrackClusterDZ      = 0.35      # Maximum distance between tracks considered as a cluster
        self.TrackMaxZ0          = 200.0    # Maximum track Z0 to be considered for vertexing
        self.TrackMaxD0          = 10.0     # Maximum track d0 to be considered for vertexing
        self.TrackMaxZ0err       = 5.0      # Maximum track Z0 error to be considered for vertexing
        self.TrackMaxD0err       = 5.0      # Maximum track d0 error to be considered for vertexing
        self.TrackMinNDF         = 2.0      # Minimum track NDF to be considered for vertexing
        self.TrackMinQual        = 0.0      # Minimum track chi^2/NDF to be considered for vertexing
        self.TrackMaxQual        = 10.0     # Maximum track chi^2/NDF to be considered for vertexing
        self.TrackMinChi2Prob    = -10.0    # Minimum track cumulative chi2 probability, from CLHEP/GenericFunctions/CumulativeChiSquare.hh 
        self.TrackMinSiHits      = 7        # Minimum # track silicon (PIX + SCT) hits to be considered for vertexing
        self.TrackMinPIXHits     = 0        # Minimum # track silicon (PIX + SCT) hits to be considered for vertexing
        self.TrackMinSCTHits     = 0        # Minimum # track silicon (PIX + SCT) hits to be considered for vertexing
        self.TrackMinTRTHits     = -10      # Minimum # track TRT hits to be considered for vertexing
        
        self.VertexMinNTrk       = 2        # Minimum # tracks in a cluster to be considered for vertexing
        self.VertexMaxNTrk       = 100      # Maximum # tracks in a cluster to be considered for vertexing (saves on time!)
        self.VertexMaxXerr       = 1.       # Maximum resulting X error on vertex fit for "good" vertices
        self.VertexMaxYerr       = 1.       # Maximum resulting Y error on vertex fit for "good" vertices
        self.VertexMaxZerr       = 10.      # Maximum resulting Z error on vertex fit for "good" vertices
        self.VertexMinQual       = 0.0      # Minimum resulting chi^2/NDF on vertex fit for "good" vertices
        self.VertexMaxQual       = 100.0    # Maximum resulting chi^2/NDF on vertex fit for "good" vertices
        self.VertexMinChi2Prob   = -10.0    # Minimum cumulative chi2 probability, from CLHEP/GenericFunctions/CumulativeChiSquare.hh      
        self.VertexBCIDMinNTrk   = 10       # Minimum # tracks in a vertex to be used for per-BCID monitoring

# Setup for relaxed cuts at 900 GeV LHC center-of-mass
class T2VertexBeamSpot_loose ( T2VertexBeamSpot_Fex ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_loose"):
        super(T2VertexBeamSpot_loose, self).__init__(name)
        self.TotalNTrackMin  = 2        # Minimum number of tracks required in an event
        self.TrackMinPt      = 0.5      # Minimum track pT to be considered for vertexing
        self.TrackSeedPt     = 0.7      # Minimum track pT to be considered for seeding a vertex fit
        self.TrackClusterDZ  = 10.0     # Maximum distance between tracks considered as a cluster
        self.TrackMinTRTHits = -10      # Minimum # track TRT hits to be considered for vertexing
        self.nSplitVertices  = 1        # Turn OFF vertex splitting
        self.vertexCollName  = "T2VertexBeamSpot_loose"

#
# Generic base classes for three differen types of chains:
# default:       never activate the output TE
# activateTE:    do activate the output TE on events that have a good vertex
# activateAllTE: indiscriminately active the output TE on all events
#

# Setup for not writing out any events
class T2VertexBeamSpot ( T2VertexBeamSpot_Fex ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot", detail=1):
        super(T2VertexBeamSpot, self).__init__(name, detail)
        self.activateTE     = False  # Activate the output TE if a good vertex is found
        self.activateAllTE  = False  # Activate the output TE for all events where this trigger is run
        self.nSplitVertices = 2      # Turn on vertex splitting
        self.vertexCollName = name

# Setup for writing out all events seen by the BeamSpot algorithm
class T2VertexBeamSpot_activeAllTE ( T2VertexBeamSpot_Fex ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE", detail=1):
        super(T2VertexBeamSpot_activeAllTE, self).__init__(name, detail)
        self.activateTE     = False  # Activate the output TE if a good vertex is found
        self.activateAllTE  = True   # Activate the output TE for all events where this trigger is run
        self.nSplitVertices = 1      # Turn OFF vertex splitting
        self.vertexCollName = name

# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm
class T2VertexBeamSpot_activeTE ( T2VertexBeamSpot_Fex ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE", detail=1):
        super(T2VertexBeamSpot_activeTE, self).__init__(name, detail)
        self.activateTE     = True    # Activate the output TE if a good vertex is found
        self.activateAllTE  = False   # Activate the output TE for active events where this trigger is run
        self.nSplitVertices = 1       # Turn OFF vertex splitting
        self.vertexCollName = name

#
# from: TrigInDetEvent/TrigInDetTrack.h
#
#  enum AlgoId{NULLID=0, SITRACKID=1, IDSCANID=2, TRTLUTID=3, TRTXKID=4, STRATEGY_A_ID=5, STRATEGY_B_ID=6, STRATEGY_C_ID=7,
#	      STRATEGY_F_ID=8, STRATEGY_G_ID=9,
#	      STRATEGY_AB_ID=10, STRATEGY_BC_ID=11, STRATEGY_AC_ID=12};

class AlgoId :
    SITRACKID=1
    IDSCANID=2
    STRATEGY_A_ID=5
    STRATEGY_B_ID=6
    STRATEGY_F_ID=8
    FTF=14

#
# L2Star Strategy A
#

# Setup for using L2StarA tracks
class T2VertexBeamSpot_L2StarA ( T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_L2StarA"):
        super(T2VertexBeamSpot_L2StarA, self).__init__(name, detail=1)
        self.TrackAlgoID    = AlgoId.STRATEGY_A_ID

# Setup for writing out all events seen by the BeamSpot algorithm with L2StarA tracks
class T2VertexBeamSpot_activeAllTE_L2StarA ( T2VertexBeamSpot_activeAllTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE_L2StarA"):
        super(T2VertexBeamSpot_activeAllTE_L2StarA, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.STRATEGY_A_ID

# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm with L2StarA tracks
class T2VertexBeamSpot_activeTE_L2StarA ( T2VertexBeamSpot_activeTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE_L2StarA"):
        super(T2VertexBeamSpot_activeTE_L2StarA, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.STRATEGY_A_ID

#
# L2Star Strategy B
#

# Setup for using L2StarB tracks
class T2VertexBeamSpot_L2StarB ( T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_L2StarB"):
        super(T2VertexBeamSpot_L2StarB, self).__init__(name, detail=2)
        self.TrackAlgoID    = AlgoId.STRATEGY_B_ID

# Setup for writing out all events seen by the BeamSpot algorithm with L2StarB tracks
class T2VertexBeamSpot_activeAllTE_L2StarB ( T2VertexBeamSpot_activeAllTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE_L2StarB"):
        super(T2VertexBeamSpot_activeAllTE_L2StarB, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.STRATEGY_B_ID

# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm with L2StarB tracks
class T2VertexBeamSpot_activeTE_L2StarB ( T2VertexBeamSpot_activeTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE_L2StarB"):
        super(T2VertexBeamSpot_activeTE_L2StarB, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.STRATEGY_B_ID

#
# L2Star Strategy F
#

# Setup for using L2StarF tracks
class T2VertexBeamSpot_L2StarF ( T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_L2StarF"):
        super(T2VertexBeamSpot_L2StarF, self).__init__(name, detail=2)
        self.TrackAlgoID    = AlgoId.STRATEGY_F_ID

# Setup for writing out all events seen by the BeamSpot algorithm with L2StarF tracks
class T2VertexBeamSpot_activeAllTE_L2StarF ( T2VertexBeamSpot_activeAllTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE_L2StarF"):
        super(T2VertexBeamSpot_activeAllTE_L2StarF, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.STRATEGY_F_ID

# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm with L2StarF tracks
class T2VertexBeamSpot_activeTE_L2StarF ( T2VertexBeamSpot_activeTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE_L2StarF"):
        super(T2VertexBeamSpot_activeTE_L2StarF, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.STRATEGY_F_ID

# Setup for using FTF tracks
class T2VertexBeamSpot_FTF ( T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_FTF"):
        super(T2VertexBeamSpot_FTF, self).__init__(name, detail=2)
        self.TrackAlgoID    = AlgoId.FTF

# Setup for writing out all events seen by the BeamSpot algorithm with FTF tracks
class T2VertexBeamSpot_activeAllTE_FTF ( T2VertexBeamSpot_activeAllTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE_FTF"):
        super(T2VertexBeamSpot_activeAllTE_FTF, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.FTF

# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm with FTF tracks
class T2VertexBeamSpot_activeTE_FTF ( T2VertexBeamSpot_activeTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE_FTF"):
        super(T2VertexBeamSpot_activeTE_FTF, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.FTF

 # Setup for using FTK tracks
class T2VertexBeamSpot_FTK ( T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_FTK"):
        super(T2VertexBeamSpot_FTK, self).__init__(name, detail=2)
        self.TrackAlgoID    = AlgoId.FTF
 
# Setup for writing out all events seen by the BeamSpot algorithm with FTK tracks
class T2VertexBeamSpot_activeAllTE_FTK ( T2VertexBeamSpot_activeAllTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE_FTK"):
        super(T2VertexBeamSpot_activeAllTE_FTK, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.FTF
 
# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm with FTK tracks
class T2VertexBeamSpot_activeTE_FTK ( T2VertexBeamSpot_activeTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE_FTK"):
        super(T2VertexBeamSpot_activeTE_FTK, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.FTF

 # Setup for using FTKRefit tracks
class T2VertexBeamSpot_FTKRefit ( T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_FTKRefit"):
        super(T2VertexBeamSpot_FTKRefit, self).__init__(name, detail=2)
        self.TrackAlgoID    = AlgoId.FTF
 
# Setup for writing out all events seen by the BeamSpot algorithm with FTK tracks
class T2VertexBeamSpot_activeAllTE_FTKRefit ( T2VertexBeamSpot_activeAllTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE_FTKRefit"):
        super(T2VertexBeamSpot_activeAllTE_FTKRefit, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.FTF
 
# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm with FTK tracks
class T2VertexBeamSpot_activeTE_FTKRefit ( T2VertexBeamSpot_activeTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE_FTKRefit"):
        super(T2VertexBeamSpot_activeTE_FTKRefit, self).__init__(name, detail=0)
        self.TrackAlgoID    = AlgoId.FTF
