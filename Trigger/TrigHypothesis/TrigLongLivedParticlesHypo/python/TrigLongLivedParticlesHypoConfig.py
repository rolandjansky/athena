# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def TrigLongLivedParticlesHypoToolFromDict( chainDict ):
    """
    Generates the Hypo Tool from the chain dictionary
    """
    name = chainDict['chainName']

    ## Initialize default-configured HypoTool as 'tool'
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConfig import MuonClusterHypoToolConfig
    tool = MuonClusterHypoToolConfig(name)

    return tool


# Monitoring Tool Configuration for HypoAlg
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import MuonClusterHypoAlg

class MuonClusterHypoAlgConfig(MuonClusterHypoAlg):
    __slots__ = []
    def __init__ (self, name="MuonClusterHypoAlgConfig"):
        super(MuonClusterHypoAlgConfig, self).__init__(name)

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import TrigMuonClusterHypoAlgMonitoring

        self.MonTool = TrigMuonClusterHypoAlgMonitoring()


# Monitoring Tool Configuration for HypoTool
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import MuonClusterHypoTool

class MuonClusterHypoToolConfig(MuonClusterHypoTool):
    __slots__ = []
    def __init__ (self, name="MuonClusterHypoToolConfig"):
        super(MuonClusterHypoToolConfig, self).__init__(name)

        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoMonitoring import TrigMuonClusterHypoToolMonitoring

        self.MonTool = TrigMuonClusterHypoToolMonitoring()
        self.acceptAll = False
