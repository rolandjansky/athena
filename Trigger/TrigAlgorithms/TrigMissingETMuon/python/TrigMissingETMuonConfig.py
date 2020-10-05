# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigMissingETMuon.TrigMissingETMuonConf import TrigMissingETMuon



class L2TrigMissingETMuon_Fex ( TrigMissingETMuon ) :
    __slots__ = []
    def __init__ (self, name="L2TrigMissingETMuon_Fex"):
        super(L2TrigMissingETMuon_Fex, self).__init__(name)

        self.METLabel = "T2MissingET"
        #self.MuonPtCut = 5.0

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 50]

class L2CaloTrigMissingETMuon_Fex ( TrigMissingETMuon ) :
    __slots__ = []
    def __init__ (self, name="L2CaloTrigMissingETMuon_Fex"):
        super(L2CaloTrigMissingETMuon_Fex, self).__init__(name)

        self.METLabel = "L2MissingET_FEB"
        #self.MuonPtCut = 5.0

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 50]

# Default fex for EF
class EFTrigMissingETMuon_Fex ( TrigMissingETMuon ) :
    __slots__ = []
    def __init__ (self, name="EFTrigMissingETMuon_Fex"):
        super(EFTrigMissingETMuon_Fex, self).__init__(name)

        self.METLabel = "TrigEFMissingET"
        #self.MuonPtCut = 5.0

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 50]

class EFTrigMissingETMuon_Fex_FEB ( TrigMissingETMuon ) :
    __slots__ = []
    def __init__ (self, name="EFTrigMissingETMuon_Fex_FEB"):
        super(EFTrigMissingETMuon_Fex_FEB, self).__init__(name)

        self.METLabel = "TrigEFMissingET_FEB"
        #self.MuonPtCut = 5.0

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 50]

class EFTrigMissingETMuon_Fex_topocl ( TrigMissingETMuon ) :
    __slots__ = []
    def __init__ (self, name="EFTrigMissingETMuon_Fex_topocl"):
        super(EFTrigMissingETMuon_Fex_topocl, self).__init__(name)

        self.METLabel = "TrigEFMissingET_topocl"
        #self.MuonPtCut = 5.0

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 50]

class EFTrigMissingETMuon_Fex_topoclPS ( TrigMissingETMuon ) :
    __slots__ = []
    def __init__ (self, name="EFTrigMissingETMuon_Fex_topoclPS"):
        super(EFTrigMissingETMuon_Fex_topoclPS, self).__init__(name)

        self.METLabel = "TrigEFMissingET_topocl_PS"
        #self.MuonPtCut = 5.0

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 50]


class EFTrigMissingETMuon_Fex_Jets ( TrigMissingETMuon ) :
    __slots__ = []
    def __init__ (self, name="EFTrigMissingETMuon_Fex_Jets"):
        super(EFTrigMissingETMuon_Fex_Jets, self).__init__(name)

        self.METLabel = "TrigEFMissingET_Jets"
        #self.MuonPtCut = 5.0

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 50]

class EFTrigMissingETMuon_Fex_topoclPUC ( TrigMissingETMuon ) :
    __slots__ = []
    def __init__ (self, name="EFTrigMissingETMuon_Fex_topoclPUC"):
        super(EFTrigMissingETMuon_Fex_topoclPUC, self).__init__(name)

        self.METLabel = "TrigEFMissingET_topocl_PUC"
        #self.MuonPtCut = 5.0

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 50]


