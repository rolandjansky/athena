# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# include the python fragment to set up the default bphysics vertex fitter
from TrigBphysHypo import TrigBphysVertexingConfig

from TrigBphysHypo.TrigBphysHypoConf import TrigMultiTrkFex

from AthenaCommon.AppMgr import ToolSvc

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigBphysHypo.TrigMultiTrkFexMonitoring import TrigMultiTrkFexValidationMonitoring
from TrigBphysHypo.TrigMultiTrkFexMonitoring import TrigMultiTrkFexOnlineMonitoring


class TrigMultiTrkFexPy (TrigMultiTrkFex):
    __slots__ = []
    def __init__(self, name = "MultiTrkFexPy"):
        super( TrigMultiTrkFex, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts

        self.AcceptAll = False


    def setTrackThresholds(self, thresholds) :
        print thresholds
        if len(thresholds) > self.nTrk :
            print "ERROR: TrigMultiTrkFex.setTrackThresholds - too many thresholds requested" , thresholds, " expecting only ", self.nTrk
        self.ptTrkMin = []  # reset, use thresholds from trigger name
        for thr in thresholds :  self.ptTrkMin.append( thr )
        while len(self.ptTrkMin) < self.nTrk  :
            self.ptTrkMin.append(900.)
            
    def setEFMuonThresholds(self, thresholds) :
        if len(thresholds) > self.nEfMuon :
            print "ERROR: TrigMultiTrkFex.setEFMuonThresholds - too many thresholds requested"
        self.ptMuonMin = []  # reset, use thresholds from trigger name
        for thr in thresholds :  self.ptMuonMin.append( thr )
        while len(self.ptMuonMin) < self.nEfMuon  :
            self.ptMuonMin.append(2000.)  # lower pt cut makes no sense at trigger

    def setL2CombMuonThresholds(self, thresholds) :
        if len(thresholds) > self.nL2CombMuon :
            print "ERROR: TrigMultiTrkFex.setL2CombMuonThresholds - too many thresholds requested"
        self.ptMuonMin = []  # reset, use thresholds from trigger name
        for thr in thresholds :  self.ptMuonMin.append( thr )
        while len(self.ptMuonMin) < self.nL2CombMuon  :
            self.ptMuonMin.append(2000.)  # lower pt cut makes no sense at trigger

    def setL2SAMuonThresholds(self, thresholds) :
        print thresholds
        # in case of L2SA limits, take only 1 muon with lowest threshold
        if len(thresholds) > self.nL2SAMuon :
            print "ERROR: TrigMultiTrkFex.setL2SAMuonThresholds - too many thresholds requested: ", thresholds, " expecting only ", self.nL2SAMuon
        self.ptMuonMin = []  # reset, use thresholds from trigger name
        for thr in thresholds :  self.ptMuonMin.append( thr )
        while len(self.ptMuonMin) < self.nL2SAMuon  :
            self.ptMuonMin.append(2000.)  # lower pt cut makes no sense at trigger


######################################
#  trkPhi selects 2 OS tracks pt>X,X GeV with M in [0.840,1.240] GeV
######################################
class TrigMultiTrkFex_trkPhi (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_trkPhi"):
        super( TrigMultiTrkFexPy, self ).__init__( name )

        #self.trackCollectionKey = "'
        self.nTrk = 2
        self.nTrkCharge = 0
        self.nTrkMassMin = [840.]  
        self.nTrkMassMax = [1240.] 
        self.ptTrkMin = [4000., 4000. ] # set minimal pt of tracks for Phi+Pi
        self.diTrkMassMin = []   # phi window
        self.diTrkMassMax = []
        self.diTrkCharge = -1
        # muons are not matched to tracks, but still require to be present in TE
        self.nEfMuon = 0
        self.nL2CombMuon = 0
        self.nL2SAMuon = 0  # as we run on muon RoIs all necessary muons are already requested.
        self.ptMuonMin = [] #[3600.]
        #self.overlapdR  = 0.005 

        time = TrigTimeHistToolConfig("Time")
        validation = TrigMultiTrkFexValidationMonitoring()
        online = TrigMultiTrkFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

###################################################################################
#  trkPhiX selects 3 tracks pt>X,X,1 GeV M<2.9GeV and OS pair, M in [0.840,1.240] GeV
#####################################################################################
class TrigMultiTrkFex_trkPhiX (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_trkPhiX"):
        super( TrigMultiTrkFexPy, self ).__init__( name )

        #self.trackCollectionKey = "'
        self.nTrk = 3
        self.nTrkCharge = 1
        self.ptTrkMin = [4000., 4000., 1000. ] # set minimal pt of tracks for Phi+Pi
        self.diTrkMassMin = [840.]   # phi window
        self.diTrkMassMax = [1240.]
        self.diTrkCharge = 0
        self.nTrkMassMin = [0.]
        self.nTrkMassMax = [2900.] # cut away J/psi
        # muons are not matched to tracks, but still require to be present in TE
        self.nEfMuon = 0
        self.nL2CombMuon = 0
        self.nL2SAMuon = 0  # as we run on muon RoIs all necessary muons are already requested.
        self.ptMuonMin = [] #[3600.]
        #self.overlapdR  = 0.005 

        time = TrigTimeHistToolConfig("Time")
        validation = TrigMultiTrkFexValidationMonitoring()
        online = TrigMultiTrkFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

###################################################################################
#  trkPhiXTight selects 3 tracks pt>X,X,1 GeV M [1.5,2.5]GeV and OS pair, M in [0.840,1.240] GeV
#####################################################################################

class TrigMultiTrkFex_trkPhiXTight (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_trkPhiXTight"):
        super( TrigMultiTrkFexPy, self ).__init__( name )
        self.nTrk = 3
        self.nTrkCharge = 1
        self.nTrkMassMin = [1500.]
        self.nTrkMassMax = [2500.] 
        self.ptTrkMin = [4000., 4000., 1000. ] # set minimal pt of tracks for Phi+Pi
        self.diTrkMassMin = [840.]   # phi window
        self.diTrkMassMax = [1240.]
        self.diTrkCharge = 0

        # muons are not matched to tracks, but still require to be present in TE
        self.nEfMuon = 0
        self.nL2CombMuon = 0
        self.nL2SAMuon = 0  # as we run on muon RoIs all necessary muons are already requested.
        self.ptMuonMin = [] #[3600.]
        #self.overlapdR  = 0.005 

        time = TrigTimeHistToolConfig("Time")
        validation = TrigMultiTrkFexValidationMonitoring()
        online = TrigMultiTrkFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]
        

class TrigMultiTrkFex_trkTau (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_trkTau"):
        super( TrigMultiTrkFexPy, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        #self.trackCollectionKey = "'
        self.nTrk = 3
        self.nTrkMassMin = [0.]
        self.nTrkMassMax = [2900.] # cut away J/psi
        self.nTrkCharge = 1
        self.ptTrkMin = [4000., 4000., 1000. ] # set minimal pt of tracks for Phi+Pi
        self.diTrkMassMin = []   # no sub-resonances
        self.diTrkMassMax = []
        # muons are not matched to tracks, but still require to be present in TE
        self.nEfMuon = 0
        self.nL2CombMuon = 0
        self.nL2SAMuon = 0
        self.ptMuonMin = [] #3600., 3600.]
        #self.overlapdR  = 0.005 


        time = TrigTimeHistToolConfig("Time")
        validation = TrigMultiTrkFexValidationMonitoring()
        online = TrigMultiTrkFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]


