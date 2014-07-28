include.block("TrigT2HistoPrmVtx/TriggerConfig_TrigT2HistoPrmVtx.py")
#
# Configure a suitable TrigT2HistoPrmVtx Algorithm instance
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
#

class TriggerConfig_TrigT2HistoPrmVtx:
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        if type == "jet":    
            self.__instname__ = "TrigT2HistoPrmVtx_jet_"
            self.__sequence__ = "TrigT2HistoPrmVtx/TrigT2HistoPrmVtx/jet"

        self.__instname__ += level


    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include( "TrigT2HistoPrmVtx/jobOfragment_TrigT2HistoPrmVtx.py")
include.block( "TrigT2HistoPrmVtx/jobOfragment_TrigT2HistoPrmVtx.py")

