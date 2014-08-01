include.block("TrigMuonHypo/TriggerConfig_MuisoHypo.py")
#
# Configure a suitable TrigmuisoHypo instance
#
# Constructor arguments:
# level, type, threshold, isIsolated
# 
# e.g. level=L2, type=muon, threshold=30, isIsolated=None
#      level=EF, type=egamma, threshold=20, isIsolated=isolated
#
# Methods:
# instanceName() : returns name of algorithm instance
# classAndInstanceName() : returns a list of strings to be entered in the sequence file. This string
#              defines the class and instance name

class TriggerConfig_MuisoHypo:
    
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        if type == "muon":
            self.__instname__ = "muiso_Muon_" + level
            self.__sequence__ = "MuisoHypo/MuisoHypo/Muon_"
        if type == "900GeV":
            self.__instname__ = "muiso_900GeV_" + level
            self.__sequence__ = "MuisoHypo/MuisoHypo/900GeV_"
        
        #self.__sequence__ += threshold

    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include ("TrigMuonHypo/jobOfragment_MuisoHypo.py")
