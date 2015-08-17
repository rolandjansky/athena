include.block("TrigBphysHypo/TriggerConfig_EFDsPhiPi.py")
#
# Configure a suitable TrigEFDsPhiPiHypo Algorithm instance
#
# Constructor arguments:
# level, type
# 
# e.g. level=EF, type=EFdsphipi
#
# Methods:
# instanceName() : returns name of algorithm instance
# classAndInstanceName() : returns a list of strings to be entered in
# the sequence file. This string defines the class and instance name

class TriggerConfig_EFDsPhiPi:
    
    def __init__(self, level, type = "EFdsphipi"):
        
        # the only instance name possible at present is L2Calo_ele_L2
        
        if type == "EFdsphipi":    
            self.__instname__ = "TrigEFDsPhiPiHypo_EFDsPhiPi_"
            self.__sequence__ = "TrigEFDsPhiPiHypo/EFDsPhiPiHypo/1"
            self.__instname__ += level
            
            
    def instanceName(self):
        return self.__instname__
            
    def classAndInstanceName(self):
        return [ self.__sequence__ ]

include( "TrigBphysHypo/jobOfragment_TrigDsPhiPi.py" )
include.block( "TrigBphysHypo/jobOfragment_TrigDsPhiPi.py" )
