include.block("TrigL2TRTSegFinder/TriggerConfig_TrigL2TRTSegFinder.py")
#
# Configure a suitable TrigMoore Algorithm instance
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
#

class TriggerConfig_TrigL2TRTSegFinder:
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        # **temp the only instance name possible at present is L2TRTSegFinder_param_L2

        if type == "muon":
            self.__instname__ = "L2TRTSegFinder_param_"
            self.__sequence__ = "TrigL2TRTSegFinder/L2TRTSegFinder/param"
        if type == "tau":
            self.__instname__ = "L2TRTSegFinder_param_"
            self.__sequence__ = "TrigL2TRTSegFinder/L2TRTSegFinder/param"
        if type == "jet":
            self.__instname__ = "L2TRTSegFinder_param_"
            self.__sequence__ = "TrigL2TRTSegFinder/L2TRTSegFinder/param"
        if type == "egamma":    
            self.__instname__ = "L2TRTSegFinder_param_"
            self.__sequence__ = "TrigL2TRTSegFinder/L2TRTSegFinder/param"
        if type == "bphysics":
            self.__instname__ = "L2TRTSegFinder_param_"
            self.__sequence__ = "TrigL2TRTSegFinder/L2TRTSegFinder/param"

        self.__instname__ += level


    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include( "TrigL2TRTSegFinder/jobOfragment_TrigL2TRTSegFinder.py" )
if GlobalFlags.InputFormat.is_bytestream():
    # Temporary fix : can't use MC truth when Reading from BS 
    L2TRTSegFinder_param_L2 = Algorithm( "L2TRTSegFinder_param_L2" )
    L2TRTSegFinder_param_L2.UseTruthMC = 0;  
