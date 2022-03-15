# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def TrigL2LongLivedParticlesHypoToolFromDict( chainDict ):
    """
    Generates the Hypo Tool from the chain dictionary
    """
    name = chainDict['chainName']

    ## Initialize default-configured HypoTool as 'tool'
    from TrigLongLivedParticlesHypo.TrigL2LongLivedParticlesHypoConfig import MuonClusterHypoToolConfig
    tool = MuonClusterHypoToolConfig(name)

    ## To customize the hypoTools on a chain-by-chain basis (chain part called 'part' is a placeholder)
    ##  (ex: different values for nRoIEndcap, nRoIBarrel, maxeta, etc),
    ##  use the following lines & change the default values to whatever you want:
    #
    # if 'part' in chainDict['chainParts'][0]:
    #   tool = MuonClusterHypoToolConfig(name)
    #   tool.m_nRoIEndCap = 4
    #   tool.m_nRoIBarrel = 3
    #   tool.m_etaMax = 2.5
    #   tool.m_etaMid = 0.7
    #

    return tool


# Monitoring Tool Configuration for HypoAlg
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import MuonClusterHypoAlg

class MuonClusterHypoAlgConfig(MuonClusterHypoAlg):
    __slots__ = []
    def __init__ (self, name="MuonClusterHypoAlgConfig"):
        super(MuonClusterHypoAlgConfig, self).__init__(name)

        from TrigLongLivedParticlesHypo.TrigL2LongLivedParticlesHypoMonitoring import TrigMuonClusterHypoAlgMonitoring
        allMon = TrigMuonClusterHypoAlgMonitoring()

        self.MonTool = allMon


# Monitoring Tool Configuration for HypoTool
from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import MuonClusterHypoTool

class MuonClusterHypoToolConfig(MuonClusterHypoTool):
    __slots__ = []
    def __init__ (self, name="MuonClusterHypoToolConfig"):
        super(MuonClusterHypoToolConfig, self).__init__(name)

        from TrigLongLivedParticlesHypo.TrigL2LongLivedParticlesHypoMonitoring import TrigMuonClusterHypoToolMonitoring
        allMon = TrigMuonClusterHypoToolMonitoring()

        self.MonTool = allMon
        self.acceptAll = False
