include.block("TrigT2CaloCommon/TriggerConfig_TrigT2Calo.py")
#
# Configure a suitable TrigMoore Algorithm instance
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

class TriggerConfig_TrigT2Calo:
    
    def __init__(self, level, type = None, threshold = None, isIsolated = None):

        # **temp the only instance name possible at present is MooHLTEFAlgo_m1_EF

        if type == "egamma":
            self.__instname__ = "T2CaloEgamma_g4_"
            self.__sequence__ = "T2CaloEgamma/T2CaloEgamma/g4"
            include( "TrigT2CaloEgamma/jobOfragment_TrigT2CaloEgamma.py" )
            include.block( "TrigT2CaloEgamma/jobOfragment_TrigT2CaloEgamma.py" )
        if type == "jet":
            self.__instname__ = "T2CaloJet_g4_"
            self.__sequence__ = "T2CaloJet/T2CaloJet/g4"
            include("TrigT2CaloJet/jobOfragment_TrigT2CaloJet.py")
            include.block("TrigT2CaloJet/jobOfragment_TrigT2CaloJet.py")

        self.__instname__ += level


    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]

