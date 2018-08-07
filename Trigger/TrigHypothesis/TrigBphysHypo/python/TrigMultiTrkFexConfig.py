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
        super( TrigMultiTrkFexPy, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts

        self.AcceptAll = False


    def setTrackThresholds(self, thresholds) :
        self.ptTrkMin = []  # reset, use thresholds from trigger name
        for thr in sorted(thresholds)  : # should should have lowest pt first, which is what we want
            if len(self.ptTrkMin) < self.nTrk :
                self.ptTrkMin.append( thr )
        while len(self.ptTrkMin) < self.nTrk  :
            self.ptTrkMin.append(900.)
            
    def setNumberAndTrackThresholds(self, nTrk, thresholds) :
        self.ptTrkMin = []  # reset, use thresholds from trigger name
        self.nTrk = nTrk
        for thr in sorted(thresholds)  : # should should have lowest pt first, which is what we want
            if len(self.ptTrkMin) < self.nTrk :
                self.ptTrkMin.append( thr )
        while len(self.ptTrkMin) < self.nTrk  :
            self.ptTrkMin.append(900.)

    def setEFMuonThresholds(self, thresholds) :
        self.ptMuonMin = []  # reset, use thresholds from trigger name
        for thr in sorted(thresholds)  : # should should have lowest pt first, which is what we want
            if len(self.ptMuonMin) < self.nEfMuon :
                self.ptMuonMin.append( thr )
        while len(self.ptMuonMin) < self.nEfMuon  :
            self.ptMuonMin.append(2000.)  # lower pt cut makes no sense at trigger

    def setElectronTrackThresholds(self, thresholds) :
        self.ptTrkMin = []  # reset, use thresholds from trigger name
        for thr in sorted(thresholds)  : # should should have lowest pt first, which is what we want
            if len(self.ptTrkMin) < self.nTrk :
                self.ptTrkMin.append( thr )
        while len(self.ptTrkMin) < self.nTrk  :
            self.ptTrkMin.append(4500.)

    def setL2CombMuonThresholds(self, thresholds) :
        self.ptMuonMin = []  # reset, use thresholds from trigger name
        for thr in sorted(thresholds)  : # should should have lowest pt first, which is what we want
            if len(self.ptMuonMin) < self.nL2CombMuon :
                self.ptMuonMin.append( thr )
        while len(self.ptMuonMin) < self.nL2CombMuon  :
            self.ptMuonMin.append(2000.)  # lower pt cut makes no sense at trigger

    def setL2SAMuonThresholds(self, thresholds) :
        # in case of L2SA limits, take only 1 muon with lowest threshold
        self.ptMuonMin = []  # reset, use thresholds from trigger name
        for thr in sorted(thresholds)  : # should should have lowest pt first, which is what we want
            if len(self.ptMuonMin) < self.nL2SAMuon :
                self.ptMuonMin.append( thr )
        while len(self.ptMuonMin) < self.nL2SAMuon  :
            self.ptMuonMin.append(2000.)  # lower pt cut makes no sense at trigger


######################################
#  trkPhi selects 2 OS tracks pt>X,X GeV with M in [0.840,1.240] GeV
######################################
class TrigMultiTrkFex_trkPhi (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_trkPhi"):
        super( TrigMultiTrkFex_trkPhi, self ).__init__( name )

        #self.trackCollectionKey = "'
        self.nTrk = 2
        self.trkMass = 105.6583745  # looking for di-muon resonances       
        self.nTrkCharge = 0
        self.nTrkMassMin = [840.]  
        self.nTrkMassMax = [1240.] 
        self.ptTrkMin = [3600., 3600. ] # set minimal pt of tracks for Phi+Pi
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
        super( TrigMultiTrkFex_trkPhiX, self ).__init__( name )

        #self.trackCollectionKey = "'
        self.nTrk = 3
        self.trkMass = 105.6583745  # looking for di-muon resonances       
        self.nTrkCharge = 1
        self.ptTrkMin = [3600., 3600., 1000. ] # set minimal pt of tracks for Phi+Pi
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
        super( TrigMultiTrkFex_trkPhiXTight, self ).__init__( name )
        self.nTrk = 3
        self.trkMass = 105.6583745  # looking for di-muon resonances       
        self.nTrkCharge = 1
        self.nTrkMassMin = [1500.]
        self.nTrkMassMax = [2500.] 
        self.ptTrkMin = [3600., 3600., 1000. ] # set minimal pt of tracks for Phi+Pi
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
        
###################################################################################

class TrigMultiTrkFex_trkTau (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_trkTau"):
        super( TrigMultiTrkFex_trkTau, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        #self.trackCollectionKey = "'
        self.maxNOutputObject = -1
        self.trkMass = 105.6583745  # looking for di-muon         
        self.nTrk = 2
        self.nTrkMassMin = [0.]
        self.nTrkMassMax = [2900.] # cut away J/psi
        self.nTrkCharge = -1
        self.ptTrkMin = [3600., 3600. ] # set minimal pt of tracks for Phi+Pi
        self.diTrkMassMin = []   # no sub-resonances
        self.diTrkMassMax = []
        # muons are not matched to tracks, but still require to be present in TE
        self.nEfMuon = 0
        self.nL2CombMuon = 0
        self.nL2SAMuon = 0
        self.ptMuonMin = [] #3600., 3600.]
        self.overlapdR  = 0.005 


        time = TrigTimeHistToolConfig("Time")
        validation = TrigMultiTrkFexValidationMonitoring()
        online = TrigMultiTrkFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

###################################################################################
class TrigMultiTrkFex_bNmu (TrigMultiTrkFexPy):
    __slots__ = []
    # lets force name setting, as it needs to match pt cuts
    def __init__(self, name, ptMuonMin ):
        super( TrigMultiTrkFex_bNmu, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        #self.trackCollectionKey = "'
        self.trkMass = 105.6583745  # looking for di-muon resonances       
        self.nTrk = -1  # no cut
        self.nTrkMassMin = []
        self.nTrkMassMax = [] # cut away J/psi
        self.nTrkCharge = -1
        self.ptTrkMin = [] # set minimal pt of tracks for Phi+Pi
        self.diTrkMassMin = []   # no sub-resonances
        self.diTrkMassMax = []
        # muons are not matched to tracks, but still could be required to be present in TE
        self.nL2CombMuon = 0
        self.nL2SAMuon = 0

        # these are 2 cuts that matters. Set to the softest 
        self.nEfMuon = len(ptMuonMin)
        self.ptMuonMin = []
        for thr in ptMuonMin :
            self.ptMuonMin.append(thr)

        time = TrigTimeHistToolConfig("Time")
        validation = TrigMultiTrkFexValidationMonitoring()
        online = TrigMultiTrkFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]



###################################################################################

#  B+ -> mu mu K+ 
#  Bd -> mu mu K*(K+ Pi-) 
#  Bs -> mu mu Phi(K+ K-) 
#  Lambda_b -> mu mu Lambda(P Pi) 
#  Bc -> mu mu Ds(Phi pi) 
#  Bc -> mu mu D+(K pi pi)
#  Bc -> mu mu D*(D0 pi),   D0 -> K- pi+
#  Bc -> mu mu D0(K pi) 


#  BcD selects following channels
# - B_c+ -> J/psi D_s+, D_s+ -> phi pi+, phi -> K+ K-
# - B_c+ -> J/psi D+, D_+ -> K- pi+ pi+
# - B_c+ -> J/psi D*+, D*+ -> D0 pi+_s, D0 -> K- pi+
# - the same decay without reconstruction of pi+_s

#####################################################################################
class TrigMultiTrkFex_B_2mu1trk (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_B_2mu1trk"):
        super( TrigMultiTrkFex_B_2mu1trk, self ).__init__( name )

        #self.trackCollectionKey = "'
        # disable any parameter by setting it to -1 or giving an empty list
        self.trkMass = 139.57018  # take pion mass for all tracks, adjust mass accordingly, if you need Kaons
        self.nTrk = 3
        self.nTrkCharge = 1
        self.nTrkVertexChi2 = 20
        self.ptTrkMin = [1000., 1000., 1000. ] # set minimal pt of tracks; first 2 thresholds will be replaced by muon thresholds in the menu
        self.diTrkMassMin = [100.]   # di-muon
        self.diTrkMassMax = [5500.]  # 
        self.diTrkCharge = 0  # set to -1 to disable
        self.nTrkMassMin = [4040]  # lower as we replaced Kaon with pion mass
        self.nTrkMassMax = [5440.] #  default cut is 
        # muons are not matched to tracks, but still could be required to be present in TE
        self.nEfMuon = 0
        self.nL2CombMuon = 0
        self.nL2SAMuon = 0  # as we run on muon RoIs all necessary muons are already requested.
        self.ptMuonMin = [] #[3600.]
        #self.overlapdR  = 0.005 

        time = TrigTimeHistToolConfig("Time")
        validation = TrigMultiTrkFexValidationMonitoring()
        online = TrigMultiTrkFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

#####################################################################################
# K*mumu
class TrigMultiTrkFex_B_2mu2trk (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_B_2mu2trk"):
        super( TrigMultiTrkFex_B_2mu2trk, self ).__init__( name )

        #self.trackCollectionKey = "'
        # disable any parameter by setting it to -1 or giving an empty list
        self.trkMass = 139.57018  # take pion mass for all tracks, adjust mass accordingly, if you need Kaons
        self.nTrk = 4
        self.nTrkCharge = 0
        self.nTrkVertexChi2 = 60
        self.ptTrkMin = [1000., 1000., 1000. ] # set minimal pt of tracks; first 2 thresholds will be replaced by muon thresholds in the menu
        #self.diTrkMassMin = [100., 359]   # di-muon , K* (600-1500) using pion hypo
        #self.diTrkMassMax = [5500., 1421.]  # 
        self.diTrkMassMin = [100., 300]   # di-muon , K* (600-1500) using pion hypo
        self.diTrkMassMax = [5500., 1400.]  # 
        self.diTrkCharge = 0  # set to -1 to disable
        self.nTrkMassMin = [4500]  #   4600-6300 if we would use Kaon mass
        self.nTrkMassMax = [6280.] #  
        # muons are not matched to tracks, but still could be required to be present in TE
        self.nEfMuon = 0
        self.nL2CombMuon = 0
        self.nL2SAMuon = 0  # as we run on muon RoIs all necessary muons are already requested.
        self.ptMuonMin = [] #[3600.]
        #self.overlapdR  = 0.005 

        time = TrigTimeHistToolConfig("Time")
        validation = TrigMultiTrkFexValidationMonitoring()
        online = TrigMultiTrkFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]


#############################################################################
class TrigMultiTrkFex_DiMu (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_DiMu"):
        super( TrigMultiTrkFex_DiMu, self ).__init__( name )
        self.nTrk = 2
        self.trkMass = 105.6583745  # looking for di-muon resonances       
        self.nTrkVertexChi2 = 20
        self.nTrkCharge = 0
        self.nTrkMassMin = [100.]
        self.nTrkMassMax = [15000.] 
        self.ptTrkMin = [3600., 3600. ] # set minimal pt of tracks for 2mu passing L1
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


class TrigMultiTrkFex_DiMu_noCut (TrigMultiTrkFex_DiMu):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_DiMu_noCut"):
        super( TrigMultiTrkFex_DiMu_noCut, self ).__init__( name )
        self.nTrkCharge = -1
        self.nTrkVertexChi2 = -1
        self.nTrkMassMin = [0.]  
        self.nTrkMassMax = [1e+8] # should be safe at LHC, no? 
        
class TrigMultiTrkFex_DiMu_noVtx_noOS (TrigMultiTrkFex_DiMu):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_DiMu_noVtx_noOS"):
        super( TrigMultiTrkFex_DiMu_noVtx_noOS, self ).__init__( name )
        self.nTrkCharge = -1
        self.nTrkVertexChi2 = -1


class TrigMultiTrkFex_DiMu_noVtx_noM_SS (TrigMultiTrkFex_DiMu):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_DiMu_noVtx_noM_SS"):
        super( TrigMultiTrkFex_DiMu_noVtx_noM_SS, self ).__init__( name )
        self.nTrkCharge = 2
        self.nTrkVertexChi2 = -1
        self.nTrkMassMin = [0.]  # OI not sure if this will work...
        self.nTrkMassMax = [1e+8] # should be safe at LHC, no? 

#############################################################################
class TrigMultiTrkFex_Vtx2 (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "TrigMultiTrkFex_Vtx2"):
        super( TrigMultiTrkFex_Vtx2, self ).__init__( name )
        self.nTrk = 2
        self.trkMass = 105.6583745  # looking for di-muon resonances       
        self.nTrkVertexChi2 = 100 # very loose here, tighter at Hypo
        self.nTrkCharge = -1
        self.nTrkMassMin = [0.]  
        self.nTrkMassMax = [1e+8] # should be safe at LHC, no? 
        self.ptTrkMin = [3600., 3600. ] # set minimal pt of tracks for 2mu passing L1
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

class TrigMultiTrkFex_Vtx3 (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "TrigMultiTrkFex_Vtx3"):
        super( TrigMultiTrkFex_Vtx3, self ).__init__( name )
        self.nTrk = 3
        self.ptTrkMin = [3600., 3600., 3600. ] # set minimal pt of tracks for 3mu passing L1

#############################################################################
class TrigMultiTrkFex_Jpsi (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_Jpsi"):
        super( TrigMultiTrkFex_Jpsi, self ).__init__( name )
        self.nTrk = 2
        self.trkMass = 105.6583745  # looking for di-muon resonances       
        self.nTrkVertexChi2 = 20
        self.nTrkCharge = 0
        self.nTrkMassMin = [2600.]
        self.nTrkMassMax = [3600.] 
        self.ptTrkMin = [3500., 3500. ] # set minimal pt of tracks for 2mu passing L1
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


class TrigMultiTrkFex_EMu (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_EMu"):
        super( TrigMultiTrkFex_EMu, self ).__init__( name )
        self.nTrk = 2
        self.trkMass = 0.5  # looking for electron-muon resonances ; ignore muon mass      
        self.nTrkVertexChi2 = 20
        self.nTrkCharge = 0
        self.nTrkMassMin = [100.]
        self.nTrkMassMax = [7000.] 
        self.ptTrkMin = [3600., 3600. ] # set minimal pt of tracks for 2mu passing L1
        self.diTrkMassMin = []   # phi window
        self.diTrkMassMax = []
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

#############################################################################
class TrigMultiTrkFex_DiE (TrigMultiTrkFexPy):
    __slots__ = []
    def __init__(self, name = "MultiTrkFex_DiE"):
        super( TrigMultiTrkFex_DiE, self ).__init__( name )
        self.nTrk = 2
        self.trkMass =  0.511  # looking for di-electron resonances       
        self.nTrkVertexChi2 = 20  # set twice worse than for muons
        self.nTrkCharge = 0
        self.nTrkMassMin = [10.]
        self.nTrkMassMax = [9000.] 
        self.ptTrkMin = [3600., 3600. ] # set minimal pt of tracks for 2mu passing L1 - changed in generateBphysicsChainDefs
        self.diTrkMassMin = []   # phi window
        self.diTrkMassMax = []
        self.diTrkCharge = -1
        self.outputTrackCollectionKey = "MultiTrkFex_DiE"
        self.bphysCollectionKey = "MultiTrkFex_DiE"
        self.maxNOutputObject = 10

        # muons are not matched to tracks, but still require to be present in TE
        self.nEfMuon = 0
        self.nL2CombMuon = 0
        self.nL2SAMuon = 0  # as we run on muon RoIs all necessary muons are already requested.
        self.ptMuonMin = [] #[3600.]
        self.overlapdR  = 0.005 

        time = TrigTimeHistToolConfig("Time")
        validation = TrigMultiTrkFexValidationMonitoring()
        online = TrigMultiTrkFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

