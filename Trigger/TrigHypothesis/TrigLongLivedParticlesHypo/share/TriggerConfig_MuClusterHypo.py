include.block("TrigLongLivedParticlesHypo/TriggerConfig_MuClusterHypo.py")

class TriggerConfig_MuClusterHypo:
    
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        if type == "muon":
            self.__instname__ = "muCluster_Muon_" + level
            self.__sequence__ = "MuClusterHypo/MuClusterHypo/Muon_"
        if type == "900GeV":
            self.__instname__ = "muCluster_900GeV_" + level
            self.__sequence__ = "MuClusterHypo/MuClusterHypo/900GeV_"
        
        #self.__sequence__ += threshold

    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include ("TrigLongLivedParticlesHypo/jobOfragment_MuClusterHypo.py")
