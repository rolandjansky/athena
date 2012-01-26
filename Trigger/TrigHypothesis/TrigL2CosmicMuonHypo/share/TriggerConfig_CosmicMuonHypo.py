include.block('TrigL2CosmicMuonHypo/TriggerConfig_CosmicMuonHypo.py')

class TriggerConfig_CosmicMuonHypo:
    def __init__(self, level, type=None, threshold=None, isIsolated=None):
        self.__instname__ = "CosmicMuonHypo_1_"
        self.__sequence__ = "CosmicMuonHypo/CosmicMuonHypo/1"
        self.__instname__ += level
    def instanceName(self):
        return self.__instname__
    def classAndInstanceName(self):
        return [ self.__sequence__ ]

theApp.Dlls += ['TrigL2CosmicMuonHypo']

