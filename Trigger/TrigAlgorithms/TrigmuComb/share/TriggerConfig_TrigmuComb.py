include.block("TrigmuComb/TriggerConfig_TrigmuComb.py")
#
# Configure a suitable TrigmuComb Algorithm instance
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

class TriggerConfig_TrigmuComb:
    
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        # **temp the only instance name possible at present is muFast_1_L2
        
        if type == "muon":
            self.__instname__ = "muComb_Muon_"
            self.__sequence__ = "muComb/muComb/Muon"
        if type == "900GeV":
            self.__instname__ = "muComb_900GeV_"
            self.__sequence__ = "muComb/muComb/900GeV"

        self.__instname__ += level


    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]


theApp.Dlls   += [ "TrigmuComb" ]


if TriggerFlags.MuonSlice.testFlag("doSITRACK"):
    Algorithm("muComb_Muon_L2").IDalgo="SITRACK"
    Algorithm("muComb_900GeV_L2").IDalgo="SITRACK"


#configure the loose algorithm
Algorithm("muComb_900GeV_L2").WinDeta   = 0.065*2.
Algorithm("muComb_900GeV_L2").WinDphi   = 0.081*2.
Algorithm("muComb_900GeV_L2").WinDzeta  = 29.*2.


