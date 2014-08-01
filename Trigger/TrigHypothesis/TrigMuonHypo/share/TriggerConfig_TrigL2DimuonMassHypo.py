include.block("TriggerRelease/TriggerConfig_TrigL2DimuonMassHypo.py")
#
# Configure a suitable _TrigL2DimuonMassHypo Algorithm instance
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

class TriggerConfig_TrigL2DimuonMassHypo:
    
    def __init__(self, level, type = "Z"):
        
        # the only instance name possible at present is L2Photon_gamma_L2
        
        if type == "Z":    
            self.__instname__ = "TrigL2DimuonMassHypo_Z_"
            self.__sequence__ = "TrigL2DimuonMassHypo/L2dimuonHypo/Z"
            self.__instname__ += level
            Algorithm(self.__instname__).LowerMassCut=70000.0
            Algorithm(self.__instname__).UpperMassCut=120000.0

        if type == "B0":    
            self.__instname__ = "TrigL2DimuonMassHypo_B0_"
            self.__sequence__ = "TrigL2DimuonMassHypo/L2dimuonHypo/B0"
            self.__instname__ += level
            Algorithm(self.__instname__).LowerMassCut=4200.0
            Algorithm(self.__instname__).UpperMassCut=6200.0

        if type == "Jpsi":    
            self.__instname__ = "TrigL2DimuonMassHypo_Jpsi_"
            self.__sequence__ = "TrigL2DimuonMassHypo/L2dimuonHypo/Jpsi"
            self.__instname__ += level
            Algorithm(self.__instname__).LowerMassCut=2100.0
            Algorithm(self.__instname__).UpperMassCut=4100.0
            
    def instanceName(self):
        return self.__instname__
            
    def classAndInstanceName(self):
        return [ self.__sequence__ ]
