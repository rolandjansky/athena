
include.block("TrigJetHypo/TriggerConfig_TrigJetHypo.py")

class TriggerConfig_TrigJetHypo:
    def __init__(self, level, type = None, threshold = None, isIsolated = None):


                if type == "jet20a":
                    self.__instname__ = "TrigEFConeJetHypo_jet20a_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFConeJetHypo/jet20a"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_jet20.py")
                if type == "jet20b":
                    self.__instname__ = "TrigEFConeJetHypo_jet20b_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFConeJetHypo/jet20b"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_jet20.py")
                if type == "jet20c":
                    self.__instname__ = "TrigEFConeJetHypo_jet20c_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFConeJetHypo/jet20c"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_jet20.py")
                if type == "jet20d":
                    self.__instname__ = "TrigEFConeJetHypo_jet20d_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFConeJetHypo/jet20d"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_jet20.py")
                if type == "ktjet20":
                    self.__instname__ = "TrigEFKtJetHypo_ktjet20_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFKtJetHypo/ktjet20"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_ktjet20.py")
                if type == "jet160":
                    self.__instname__ = "TrigEFJetHypo_jet160_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFJetHypo/jet160"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_jet160.py")
                if type == "2jet120":
                    self.__instname__ = "TrigEFJetHypo_2jet120_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFJetHypo/2jet120"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_2jet120.py")
                if type == "3jet65":
                    self.__instname__ = "TrigEFJetHypo_3jet65_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFJetHypo/3jet65"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_3jet65.py")
                if type == "4jet50":
                    self.__instname__ = "TrigEFJetHypo_4jet50_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFJetHypo/4jet50"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_4jet50.py")
                if type == "frjet":
                    self.__instname__ = "TrigEFJetHypo_frjet_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFJetHypo/frjet"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_frjet.py")
                if type == "fljet":
                    self.__instname__ = "TrigEFJetHypo_fljet_"
                    self.__sequence__ = "TrigEFJetHypo/TrigEFJetHypo/fljet"
                    include ("TrigJetHypo/jobOfragment_EF_TrigEFJetHypo_fljet.py")
                                                                                                                                                
                self.__instname__ += level


    def instanceName(self):
        return self.__instname__

    def classAndInstanceName(self):
        return [ self.__sequence__ ]
