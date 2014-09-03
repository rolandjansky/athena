include.block("TrigEgammaRec/TriggerConfig_TrigEgammaRec.py")
#
# Configure a suitable TrigEgammaRec Algorithm instance
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

class TriggerConfig_TrigEgammaRec:
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        if type == "egamma":    
            self.__instname__ = "TrigEgammaRec_g15_"
            self.__sequence__ = "TrigEgammaRec/TrigEgammaRec/g15"
            self.__instname__ += level
            include( "TrigEgammaRec/jobOfragment_TrigEgammaRec.py" )
            include.block( "TrigEgammaRec/jobOfragment_TrigEgammaRec.py" )

        if type == "gamma":    
            self.__instname__ = "TrigEgammaRec_gamma_"
            self.__sequence__ = "TrigEgammaRec/TrigEgammaRec/gamma"
            self.__instname__ += level
            include( "TrigEgammaRec/jobOfragment_TrigEgammaRec_gamma.py" )
            include.block( "TrigEgammaRec/jobOfragment_TrigEgammaRec_gamma.py" )


    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]

