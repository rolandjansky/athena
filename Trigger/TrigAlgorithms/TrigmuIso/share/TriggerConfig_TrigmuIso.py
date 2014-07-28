include.block("TrigmuIso/TriggerConfig_TrigmuIso.py")
#
# Configure a suitable TrigmuIso Algorithm instance
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

class TriggerConfig_TrigmuIso:
    
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        if type == "muon":
           self.__instname__ = "muIso_Muon_"
           self.__sequence__ = "muIso/muIso/Muon"
        if type == "900GeV":
           self.__instname__ = "muIso_900GeV_"
           self.__sequence__ = "muIso/muIso/900GeV"

        self.__instname__ += level

    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include( "TrigmuIso/muIsoJobOptions.py" )
include.block( "TrigmuIso/muIsoJobOptions.py" )
