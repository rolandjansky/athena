
include.block("TrigJetHypo/TriggerConfig_L2_TrigJetHypo.py")

class TriggerConfig_L2_TrigJetHypo:
    def __init__(self, level, type = None, threshold = None, isIsolated = None):


                if type == "jet20a":
                    self.__instname__ = "TrigL2JetHypo_jet20a_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/jet20a"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_j20.py")
                if type == "jet20b":
                    self.__instname__ = "TrigL2JetHypo_jet20b_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/jet20b"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_j20.py")
                if type == "jet20c":
                    self.__instname__ = "TrigL2JetHypo_jet20c_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/jet20c"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_j20.py")
                if type == "jet20d":
                    self.__instname__ = "TrigL2JetHypo_jet20d_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/jet20d"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_j20.py")
                if type == "jet20kt":
                    self.__instname__ = "TrigL2JetHypo_jet20kt_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/jet20kt"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_j20.py")
                if type == "jet160":
                    self.__instname__ = "TrigL2JetHypo_jet160_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/jet160"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_jet160.py")
                if type == "2jet120":
                    self.__instname__ = "TrigL2JetHypo_2jet120_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/2jet120"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_2jet120.py")
                if type == "3jet65":
                    self.__instname__ = "TrigL2JetHypo_3jet65_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/3jet65"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_3jet65.py")
                if type == "4jet50":
                    self.__instname__ = "TrigL2JetHypo_i11_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/i11"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_4jet50.py")
                if type == "frjet":
                    self.__instname__ = "TrigL2JetHypo_frjet_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/frjet"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_frjet.py")
                if type == "fljet":
                    self.__instname__ = "TrigL2JetHypo_fljet_"
                    self.__sequence__ = "TrigL2JetHypo/TrigL2JetHypo/fljet"
                    include ("TrigJetHypo/jobOfragment_L2_TrigL2JetHypo_fljet.py")
                                                                                

                self.__instname__ += level


    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]
