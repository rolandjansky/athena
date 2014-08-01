include.block("TriggerRelease/TriggerConfig_TrigMooreHypo.py")
#
# Configure a suitable TrigMooreHypo instance
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

class TriggerConfig_TrigMooreHypo:
    
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        # **temp the only instance name possible at present is muFast_1_L2

        self.__instname__ = "mufast_" + threshold + "_" + level

        self.__sequence__ = "TrigMooreHypo/TrigMooreHypo/"
        self.__sequence__ += threshold

    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include ("TrigMuonHypo/jobOfragment_MooreHypo.py")    
