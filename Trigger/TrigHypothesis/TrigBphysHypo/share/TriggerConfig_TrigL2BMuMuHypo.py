include.block("TriggerRelease/TriggerConfig_TrigL2BMuMuHypo.py")
#
# Configure a suitable _TrigL2BMuMuHypo Algorithm instance
#
# Constructor arguments:
# level, type
# 
# e.g. level=L2, type=Z
#
# Methods:
# instanceName() : returns name of algorithm instance
# classAndInstanceName() : returns a list of strings to be entered in
# the sequence file. This string defines the class and instance name

class TriggerConfig_TrigL2BMuMuHypo:
    
    def __init__(self, level, type = "B0"):
        
        if type == "B0":    
            self.__instname__ = "TrigL2BMuMuHypo_B0_"
            self.__sequence__ = "TrigL2BMuMuFex/TrigL2BMuMuFex/B0 TrigL2BMuMuHypo/TrigL2BMuMuHypo/B0"
            self.__instname__ += level
            Algorithm(self.__instname__).LowerMassCut=4200.0
            Algorithm(self.__instname__).UpperMassCut=6200.0

    def instanceName(self):
        return self.__instname__
            
    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include( "TrigBphysHypo/jobOfragment_TrigL2BMuMuHypo.py" )
include.block( "TrigBphysHypo/jobOfragment_TrigL2BMuMuHypo.py" )
