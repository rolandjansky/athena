# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import MuonCluster
from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import TrigMuonJetFex
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING,ERROR


class MuonClusterConfig(MuonCluster): 
   __slots__ = []
   def __init__ (self, name="MuonClusterConfig", IDtracking="STRATEGY_B"): 
        super(MuonClusterConfig, self).__init__(name)

        from TrigL2LongLivedParticles.TrigL2LongLivedParticlesMonitoring import TrigMuonClusterValidationMonitoring, TrigMuonClusterOnlineMonitoring, TrigMuonClusterCosmicMonitoring
        validation = TrigMuonClusterValidationMonitoring()
        online     = TrigMuonClusterOnlineMonitoring()
        cosmic     = TrigMuonClusterCosmicMonitoring()

 
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("muClu_Time")
 
        self.AthenaMonTools = [ validation, online, time, cosmic]
 
        # muClu Parameters
        self.IDalgo      = IDtracking
        self.DeltaR      = 0.4
        self.DeltaRJet   = 0.7
        self.DeltaRTrk   = 0.4
        self.OutputLevel = INFO       
       

class TrigL2MuonJetFexGlobal (TrigMuonJetFex):
    __slots__ = []
    def __init__(self, name):
        super( TrigL2MuonJetFexGlobal, self ).__init__( name )
        
        self.Instance                    = "L2"



class TrigEFMuonJetFexGlobal (TrigMuonJetFex):
    __slots__ = []
    def __init__(self, name):
        super( TrigEFMuonJetFexGlobal, self ).__init__( name )

        self.Instance       = "EF"




class L2MuonJetFex (TrigL2MuonJetFexGlobal):
    __slots__ = []
    def __init__(self, name = "L2MuonJetFex"):
        super( L2MuonJetFex, self ).__init__( name )


class EFMuonJetFex (TrigEFMuonJetFexGlobal):
    __slots__ = []
    def __init__(self, name = "EFMuonJetFex"):
        super( EFMuonJetFex, self ).__init__( name )

                                                               
