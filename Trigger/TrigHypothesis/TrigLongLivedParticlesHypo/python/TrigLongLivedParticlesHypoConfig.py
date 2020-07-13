# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf as Conf
from AthenaCommon.SystemOfUnits import GeV


def getCaloRatioHypoInstance( instance, threshold, logratio, dotrackiso):

    if(logratio>0):
        name=instance+"_"+str(threshold)+"GeV"

    if(logratio<0):
        if (dotrackiso):
            name=instance+"_"+str(threshold)+"GeV_reversed"
        else:
            name=instance+"_"+str(threshold)+"GeV_reversed_notrackiso"

    return CaloRatioHypo( threshold=threshold, 
                          logratio=logratio, 
                          dotrackiso=dotrackiso, 
                          name=name )


class MuonClusterHypoConfig (Conf.MuonClusterHypo):
    __slots__ = []
    def __init__(self, name, maxEta, midEta):

        super( MuonClusterHypoConfig, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll           = False
        self.nRoIEndCap          = 4 
        self.nRoIBarrel          = 3 
        self.maxEta              = maxEta
        self.midEta              = midEta

class MuonClusterIsolationHypoConfig (Conf.MuonClusterIsolationHypo):
    __slots__ = []
    def __init__(self, name, maxEta, midEta, numJet, numTrk, doIsolation):
        super( MuonClusterIsolationHypoConfig, self ).__init__( name )

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import MuonClusterHypoOnlineMonitoring,MuonClusterHypoValidationMonitoring,MuonClusterHypoCosmicMonitoring
        validation = MuonClusterHypoValidationMonitoring()
        online     = MuonClusterHypoOnlineMonitoring()
        cosmic     = MuonClusterHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("MuonClusterIsolationHypo_Time")

        self.AthenaMonTools = [ validation, online, time, cosmic]

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll           = False
        self.nRoIEndCap          = 4
        self.nRoIBarrel          = 3
        self.maxEta              = maxEta
        self.midEta              = midEta
        self.nJet                = numJet
        self.nTrk                = numTrk
        self.doIsolation         = doIsolation

class L2HVJetHypoAllCutsBase (Conf.TrigL2HVJetHypoAllCuts):
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


class L2HVJetHypoBase (Conf.TrigL2HVJetHypo):
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


class L2HVJetHypoTrkBase (Conf.TrigL2HVJetHypoTrk):
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


class TrigNewLoFHypoConfig (Conf.TrigNewLoFHypo):
    __slots__ = []
    def __init__(self, name = "TrigNewLoFHypoConfig"):
        super( TrigNewLoFHypoConfig, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll           = False
        self.LoFCellContSize     = 4


class TrigLoFRemovalHypoConfig (Conf.TrigLoFRemovalHypo):
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


class CaloRatioHypo (Conf.TrigCaloRatioHypo):
    __slots__ = []
    def __init__(self, threshold, logratio, dotrackiso, name):
        super( CaloRatioHypo, self ).__init__( name )

        self.EtCut       = float(threshold)*GeV
        self.LogRatioCut = logratio

        if(logratio>0):
            self.PtMinID     = 2*GeV
            self.TrackCut    = 0
            self.DeltaR      = 0.2
            self.EtaCut      = 2.5
            self.Reversed    = False
            self.DoTrackIso  = True

        if(logratio<0):
            self.PtMinID     = 2*GeV
            self.TrackCut    = 0
            self.DeltaR      = 0.4
            self.EtaCut      = 2.5
            self.Reversed    = True
            self.DoTrackIso  = dotrackiso
        
        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import TrigCaloRatioHypoValidationMonitoring, TrigCaloRatioHypoOnlineMonitoring, TrigCaloRatioHypoCosmicMonitoring
        validation = TrigCaloRatioHypoValidationMonitoring()
        online = TrigCaloRatioHypoOnlineMonitoring()
        cosmic = TrigCaloRatioHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("CaloRatioHypo_Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]


