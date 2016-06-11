# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import *
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import *
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.AppMgr import ToolSvc



class MuonClusterHypoConfig (MuonClusterHypo):
    __slots__ = []
    def __init__(self, name, maxEta, numJet, numTrk):
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
        self.nJet                = numJet
        self.nTrk                = numTrk



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


class TrigNewLoFHypoConfig (TrigNewLoFHypo):
    __slots__ = []
    def __init__(self, name = "TrigNewLoFHypoConfig"):
        super( TrigNewLoFHypoConfig, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll           = False
        self.LoFCellContSize     = 4


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


class CaloRatioHypo (TrigCaloRatioHypo):
    __slots__ = []
    def __init__(self, name, threshold, logratio):
        super( CaloRatioHypo, self ).__init__( name )

        self.EtCut       = threshold
        self.LogRatioCut = logratio


class ReversedCaloRatioHypo (TrigCaloRatioHypo):
    __slots__ = []
    def __init__(self, name, threshold, logratio):
        super( ReversedCaloRatioHypo, self ).__init__( name )

        self.EtCut       = threshold
        self.LogRatioCut = logratio
        self.Reversed    = True


        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import TrigCaloRatioHypoValidationMonitoring, TrigCaloRatioHypoOnlineMonitoring, TrigCaloRatioHypoCosmicMonitoring
        validation = TrigCaloRatioHypoValidationMonitoring()
        online = TrigCaloRatioHypoOnlineMonitoring()
        cosmic = TrigCaloRatioHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("CaloRatioHypo_Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]


