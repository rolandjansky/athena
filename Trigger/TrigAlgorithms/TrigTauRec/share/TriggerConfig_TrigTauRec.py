include.block("TrigTauRec/TriggerConfig_TrigTauRec.py")
#
# Configure a suitable TrigTauRec Algorithm instance
#
# Constructor arguments:
# level, type, threshold, isIsolated
#
# e.g. level=L2, type=muon, threshold=30, isIsolated=None
#      level=EF, type=egamma, threshold=20, isIsolated=isolated
#
# Methods:
# instanceName() : returns name of algorithm instance
# classAndInstanceName() : returns a string to be entered in the sequence file. This string
#              defines the class and instance name
#


class TriggerConfig_TrigTauRec:
    def __init__(self, type = None, threshold = None, isIsolated = None):

        # currently only one option defined
        
        self.__instname__ = "TrigTauRec_h5_EF"
        self.__sequence__ = "TrigTauRec/TrigTauRec/h5"
        
    def instanceName(self):
        return self.__instname__
        
    def classAndInstanceName(self):
        return [ self.__sequence__ ]
        
if TriggerFlags.TauSlice.doEFID() and TriggerFlags.TauSlice.doEFCalo():
    include("TrigTauRec/jobOfragment_TrigTauRec.py")
elif TriggerFlags.TauSlice.doEFCalo():
    include("TrigTauRec/jobOfragment_TrigTauRec_noID.py")
