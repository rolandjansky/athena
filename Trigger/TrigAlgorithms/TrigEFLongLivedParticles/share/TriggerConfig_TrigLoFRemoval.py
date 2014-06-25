include.block("TrigEFLongLivedParticles/TriggerConfig_TrigLoFRemoval.py")

class TriggerConfig_TrigLoFRemoval:
    
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        if type == "jet":
           self.__instname__ = "TrigLoFRemoval_Jet_"
           self.__sequence__ = "TrigLoFRemoval/TrigLoFRemoval/Jet"

        self.__instname__ += level

    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include( "TrigEFLongLivedParticles/TrigLoFRemovalJobOptions.py" )
include.block( "TrigEFLongLivedParticles/TrigLoFRemovalJobOptions.py" )
