# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import MuonCluster

class MuonClusterConfig(MuonCluster):
    __slots__ = []
    def __init__ (self, name="MuonClusterConfig"):
        super(MuonClusterConfig, self).__init__(name)

        from TrigL2LongLivedParticles.TrigL2LongLivedParticlesMonitoring import TrigMuonClusterAlgorithmMonitoring
        montool = TrigMuonClusterAlgorithmMonitoring()

        self.MonTool = montool

        # muClu Parameters
        self.DeltaR      = 0.4
