# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import MuonCluster
from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import MuonClusterIsolation
from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import TrigMuonJetFex
from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import TrigJetSplitter
from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import TrigBHremoval


def getJetSplitterInstance( instance, logratio, pufixlogratio ):

    name=instance+"_"+str(int(pufixlogratio*10))

    return JetSplitter( name=name,
                        logratio=logratio,
                        pufixlogratio=pufixlogratio
                        )

def getJetSplitterInstance_LowLogRatio( ):
    return JetSplitter_LowLogRatio( name="JetSplitter_LowLogRatio" )


class JetSplitter (TrigJetSplitter):
    __slots__ = []
    
    def __init__(self, name, logratio, pufixlogratio):
        super( JetSplitter, self ).__init__( name )
        
        self.JetInputKey  = "TrigJetRec"
        self.JetOutputKey = "SplitJet"
        self.EtaHalfWidth = 0.4
        self.PhiHalfWidth = 0.4
        self.JetLogRatio     = logratio
        self.JetPUFixLogRatio     = pufixlogratio

class JetSplitter_LowLogRatio (TrigJetSplitter):
    __slots__ = []
    
    def __init__(self, name):
        super( JetSplitter_LowLogRatio, self ).__init__( name )
        
        self.JetInputKey  = "TrigJetRec"
        self.JetOutputKey = "SplitJet_LowLogRatio"
        self.EtaHalfWidth = 0.4
        self.PhiHalfWidth = 0.4
        self.JetLogRatio  = -1.7
        self.Reversed     = True


def getBHremovalInstance( ):
    return BHremoval ( name="BHremoval" )


class BHremoval (TrigBHremoval):
    __slots__ = []
    
    def __init__(self, name="BHremoval"):
        super( BHremoval, self ).__init__( name )
        
        self.JetInputKey  = "TrigJetRec"
        self.JetOutputKey = "SplitJet"
        self.EtaHalfWidth = 0.4
        self.PhiHalfWidth = 0.4
        self.JetLogRatio  = 1.2

class MuonClusterConfig(MuonCluster): 
   __slots__ = []
   def __init__ (self, name="MuonClusterConfig"): 
        super(MuonClusterConfig, self).__init__(name)

        from TrigL2LongLivedParticles.TrigL2LongLivedParticlesMonitoring import TrigMuonClusterValidationMonitoring, TrigMuonClusterOnlineMonitoring, TrigMuonClusterCosmicMonitoring
        validation = TrigMuonClusterValidationMonitoring()
        online     = TrigMuonClusterOnlineMonitoring()
        cosmic     = TrigMuonClusterCosmicMonitoring()

 
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("muClu_Time")
 
        self.AthenaMonTools = [ validation, online, time, cosmic]
 
        # muClu Parameters
        self.DeltaR      = 0.4

class MuonClusterIsolationConfig(MuonClusterIsolation): 
   __slots__ = []
   def __init__ (self, name="MuonClusterIsolationConfig"): 
        super(MuonClusterIsolationConfig, self).__init__(name)

        from TrigL2LongLivedParticles.TrigL2LongLivedParticlesMonitoring import TrigMuonClusterValidationMonitoring, TrigMuonClusterOnlineMonitoring, TrigMuonClusterCosmicMonitoring
        validation = TrigMuonClusterValidationMonitoring()
        online     = TrigMuonClusterOnlineMonitoring()
        cosmic     = TrigMuonClusterCosmicMonitoring()

 
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("muClu_Time")
 
        self.AthenaMonTools = [ validation, online, time, cosmic]
 
        # muClu Parameters
        self.DeltaR      = 0.4

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

                                                               
