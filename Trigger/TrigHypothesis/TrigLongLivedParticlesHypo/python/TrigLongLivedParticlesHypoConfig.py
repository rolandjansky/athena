# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import MuonClusterHypo
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import TrigL2HVJetHypoAllCuts
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import TrigL2HVJetHypo 
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import TrigL2HVJetHypoTrk 
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import TrigMuonJetHypo 
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import TrigLoFRemovalHypo
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import *
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.AppMgr import ToolSvc


def getTrigMuonJetHypoInstance( instance, version ):
    return MuonJetHypo( instance=instance, version=version, name="Trig"+instance+"MuonJetHypo_"+version )



class MuonJetHypo (TrigMuonJetHypo):
    
    __slots__ = []
    def __init__(self, instance, version, name):
        super( MuonJetHypo, self ).__init__( name )
        
        AllowedInstances = ["L2","EF"]
        AllowedVersions  = ["2012","2011"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None
        
        self.Instance = instance

        self.IDalgo = "None"
       
        if version=="2011" :
            self.IDalgo     = "SITRACK"
            self.UseL2Muons = False
        elif version=="2012" :
            self.IDalgo = "STRATEGY_B"
            self.UseL2Muons = True            
       
        if self.IDalgo=="None" :
            mlog.error("AlgoId is wrongly set!")
            return None
        
        if instance == "L2" :
            self.AcceptAll      = False
            self.deltaRMatching = True
            self.Instance       = "L2"
            

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import MuonJetHypoValidationMonitoring, MuonJetHypoOnlineMonitoring, MuonJetHypoCosmicMonitoring
        validation = MuonJetHypoValidationMonitoring()
        online     = MuonJetHypoOnlineMonitoring()
        cosmic     = MuonJetHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigMuonJetFex")
        time.TimerHistLimits = [0,2]
        self.AthenaMonTools = [ time, validation, online, cosmic ]



class MuonClusterHypoConfig (MuonClusterHypo):
    __slots__ = []

    def __init__(self, name = "MuonClusterHypoConfig", maxEta=1.0):
        super( MuonClusterHypoConfig, self ).__init__( name )

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import MuonClusterHypoOnlineMonitoring,MuonClusterHypoValidationMonitoring,MuonClusterHypoCosmicMonitoring
        validation = MuonClusterHypoValidationMonitoring()
        online     = MuonClusterHypoOnlineMonitoring()
        cosmic     = MuonClusterHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("MuonClusterHypo_Time")

        self.AthenaMonTools = [ validation, online, time, cosmic]

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll           = False
        self.nRoIEndCap          = 4 
        self.nRoIBarrel          = 3 
        self.nEta                = maxEta
        self.nJet                = 0 
        self.nTrk                = 0

class MuonClusterAllMSHypoConfig (MuonClusterHypo):
    __slots__ = []

    def __init__(self, name = "MuonClusterAllMSHypoConfig"):
        super( MuonClusterAllMSHypoConfig, self ).__init__( name )

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import MuonClusterHypoOnlineMonitoring,MuonClusterHypoValidationMonitoring,MuonClusterHypoCosmicMonitoring
        validation = MuonClusterHypoValidationMonitoring()
        online     = MuonClusterHypoOnlineMonitoring()
        cosmic     = MuonClusterHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("MuonClusterHypo_Time")

        self.AthenaMonTools = [ validation, online, time, cosmic]

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll           = False
        self.nRoIEndCap          = 4
        self.nRoIBarrel          = 3
        self.nEta                = 2.5
        self.nJet                = 0
        self.nTrk                = 0


class L2HVJetHypoAllCutsBase (TrigL2HVJetHypoAllCuts):
    __slots__ = []
    def __init__(self, name):
        super( L2HVJetHypoAllCutsBase, self ).__init__( name )

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import TrigL2HVJetHypoAllCutsValidationMonitoring, TrigL2HVJetHypoAllCutsOnlineMonitoring, TrigL2HVJetHypoAllCutsCosmicMonitoring
        validation = TrigL2HVJetHypoAllCutsValidationMonitoring()
        online = TrigL2HVJetHypoAllCutsOnlineMonitoring()
        cosmic = TrigL2HVJetHypoAllCutsCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2HVJetHypo_Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]

class L2HVJetHypoAllCuts (L2HVJetHypoAllCutsBase):
    __slots__ = []
    def __init__(self, name = "L2HVJetHypoAllCuts",l2_thr=35*GeV, l2_lrat=1.):
        super( L2HVJetHypoAllCuts, self ).__init__( name )

        self.Etcut_L2     = l2_thr
        self.LRaticout_L2 = l2_lrat
        self.doHECfnLeadingCellsCleaning = True
        self.doEMfJetQualityCleaning = True
        self.doHECfJetQualityCleaning = True
        self.doJetTimeCleaning = True
        self.doLowEMfCleaning = False


class L2HVJetHypoAllCuts_doCleaning (L2HVJetHypoAllCuts):
    __slots__ = []
    def __init__(self, name = "L2HVJetHypoAllCuts_doCleaning",l2_thr=35*GeV):
        super( L2HVJetHypoAllCuts_doCleaning, self ).__init__( name )

        self.Etcut_L2 = l2_thr
        self.doHECfnLeadingCellsCleaning = True
        self.doEMfJetQualityCleaning = True
        self.doHECfJetQualityCleaning = True
        self.doJetTimeCleaning = True
        self.doLowEMfCleaning = False
        self.leadingCellsThr = 5
        self.hecfThrN = 0.8
        self.hecfThrQ = 0.3
        self.jetQualityThrH = 0.3
        self.jetQualityThrE = 0.6
        self.emfThrQ = 0.90
        self.emfThrE = 0.01
        self.jetTimeCellsThr = 25




class L2HVJetHypoBase (TrigL2HVJetHypo):
    __slots__ = []
    def __init__(self, name):
        super( L2HVJetHypoBase, self ).__init__( name )

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import TrigL2HVJetHypoValidationMonitoring, TrigL2HVJetHypoOnlineMonitoring, TrigL2HVJetHypoCosmicMonitoring
        validation = TrigL2HVJetHypoValidationMonitoring()
        online = TrigL2HVJetHypoOnlineMonitoring()
        cosmic = TrigL2HVJetHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2HVJetHypo_Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]

class L2HVJetHypo (L2HVJetHypoBase):
    __slots__ = []
    def __init__(self, name = "L2HVJetHypo",l2_thr=35*GeV, l2_lrat=1.):
        super( L2HVJetHypo, self ).__init__( name )

        self.Etcut_L2     = l2_thr
        self.LRaticout_L2 = l2_lrat
        self.doHECfnLeadingCellsCleaning = True
        self.doEMfJetQualityCleaning = True
        self.doHECfJetQualityCleaning = True
        self.doJetTimeCleaning = True
        self.doLowEMfCleaning = False


class L2HVJetHypo_doCleaning (L2HVJetHypo):
    __slots__ = []
    def __init__(self, name = "L2HVJetHypo_doCleaning",l2_thr=35*GeV):
        super( L2HVJetHypo_doCleaning, self ).__init__( name )

        self.Etcut_L2 = l2_thr
        self.doHECfnLeadingCellsCleaning = True
        self.doEMfJetQualityCleaning = True
        self.doHECfJetQualityCleaning = True
        self.doJetTimeCleaning = True
        self.doLowEMfCleaning = False
        self.leadingCellsThr = 5
        self.hecfThrN = 0.8
        self.hecfThrQ = 0.3
        self.jetQualityThrH = 0.3
        self.jetQualityThrE = 0.6
        self.emfThrQ = 0.90
        self.emfThrE = 0.01
        self.jetTimeCellsThr = 25

class L2HVJetHypoTrkBase (TrigL2HVJetHypoTrk):
    __slots__ = []
    def __init__(self, name):
        super( L2HVJetHypoTrkBase, self ).__init__( name )

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import TrigL2HVJetHypoTrkValidationMonitoring, TrigL2HVJetHypoTrkOnlineMonitoring, TrigL2HVJetHypoTrkCosmicMonitoring
        validation = TrigL2HVJetHypoTrkValidationMonitoring()
        online = TrigL2HVJetHypoTrkOnlineMonitoring()
        cosmic = TrigL2HVJetHypoTrkCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2HVJetHypoTrk_Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]

class L2HVJetHypoTrk (L2HVJetHypoTrkBase):
    __slots__ = []
    def __init__(self, name = "L2HVJetHypoTrk"):
        super( L2HVJetHypoTrk, self ).__init__( name )



class TrigL2MuonJetHypoBase (TrigMuonJetHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigL2MuonJetHypoBase, self ).__init__( name )

        self.AcceptAll                   = False
        self.deltaRMatching              = True
        self.Instance                    = "L2"

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import MuonJetHypoValidationMonitoring, MuonJetHypoOnlineMonitoring, MuonJetHypoCosmicMonitoring
        validation = MuonJetHypoValidationMonitoring()
        online     = MuonJetHypoOnlineMonitoring()
        cosmic     = MuonJetHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigMuonJetFex")
        time.TimerHistLimits = [0,2]
        self.AthenaMonTools = [ time, validation, online, cosmic ]





class TrigEFMuonJetHypoBase (TrigMuonJetHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigEFMuonJetHypoBase, self ).__init__( name )

        self.Instance       = "EF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigMuonJetFex")
        time.TimerHistLimits = [0,2]
        self.AthenaMonTools = [ time ]

         
class L2MuonJetHypo (TrigL2MuonJetHypoBase):
    __slots__ = []
    def __init__(self, name = "L2MuonJetHypo"):
        super( L2MuonJetHypo, self ).__init__( name )

class EFMuonJetHypo (TrigEFMuonJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFMuonJetHypo"):
        super( EFMuonJetHypo, self ).__init__( name )

class TrigLoFRemovalHypoConfig (TrigLoFRemovalHypo):
    __slots__ = []

    def __init__(self, name = "TrigLoFRemovalHypoConfig"):
        super( TrigLoFRemovalHypoConfig, self ).__init__( name )

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import TrigLoFRemovalHypoOnlineMonitoring,TrigLoFRemovalHypoValidationMonitoring,TrigLoFRemovalHypoCosmicMonitoring
        validation = TrigLoFRemovalHypoValidationMonitoring()
        online     = TrigLoFRemovalHypoOnlineMonitoring()
        cosmic     = TrigLoFRemovalHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TrigLoFRemovalHypo_Time")

        self.AthenaMonTools = [ validation, online, time, cosmic]

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll           = False
        self.LoFCellContSize     = 4



