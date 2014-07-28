include.block("TrigL2LongLivedParticles/TriggerConfig_TrigMuClustero.py")

class TriggerConfig_TrigMuCluster:
    
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        if type == "muon":
           self.__instname__ = "MuCluster_Muon_"
           self.__sequence__ = "MuCluster/MuCluster/Muon"
        if type == "900GeV":
           self.__instname__ = "MuCluster_900GeV_"
           self.__sequence__ = "MuCluster/MuCluster/900GeV"

        self.__instname__ += level

    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include( "TrigL2LongLivedParticles/MuClusterJobOptions.py" )
include.block( "iTrigL2LongLivedParticles/MuClusterJobOptions.py" )
