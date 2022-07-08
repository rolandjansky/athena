# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TrigLongLivedParticles.TrigLongLivedParticlesConf import MuonCluster

class MuonClusterConfig(MuonCluster):
    __slots__ = []
    def __init__ (self, name="MuonClusterConfig"):
        super(MuonClusterConfig, self).__init__(name)

        from TrigLongLivedParticles.TrigLongLivedParticlesMonitoring import TrigMuonClusterAlgorithmMonitoring

        self.MonTool = TrigMuonClusterAlgorithmMonitoring()

        # muClu Parameters
        self.DeltaR      = 0.4
