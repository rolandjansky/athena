# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigL2DEtaFex, TrigEFDEtaFex

from TrigJetHypo.TrigJetDEtaFexMonitoring import TrigL2JetDEtaFexValidationMonitoring, TrigL2JetDEtaFexOnlineMonitoring
from TrigJetHypo.TrigJetDEtaFexMonitoring import TrigEFJetDEtaFexValidationMonitoring, TrigEFJetDEtaFexOnlineMonitoring

class L2DEtaFex(TrigL2DEtaFex):
    __slots__ = []
    def __init__(self, name = "L2DEtaFex", l2_thr=2.):
        super(L2DEtaFex, self).__init__( name )
        validation = TrigL2JetDEtaFexValidationMonitoring()
        online = TrigL2JetDEtaFexOnlineMonitoring() 
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2JetDEtaFex_Time")
        self.AthenaMonTools = [time, validation, online ]
        self.DEtaCut=l2_thr

class L2MjjDEta2J15Fex(TrigL2DEtaFex):
    __slots__ = []
    def __init__(self, name = "L2MjjDEta2J15Fex", l2_thr=2.5, mjj=0):
        super(L2MjjDEta2J15Fex, self).__init__( name )
        self.DEtaCut=l2_thr
        self.MjjCut=mjj
        self.EtCut=15000
        self.CheckMostForwardBackwardPair=True
        validation = TrigL2JetDEtaFexValidationMonitoring()
        online = TrigL2JetDEtaFexOnlineMonitoring()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2JetDEtaFex_Time")
        self.AthenaMonTools = [time, validation, online]

        
class EFDEtaFex(TrigEFDEtaFex):
    __slots__ = []
    def __init__(self, name = "EFDEtaFex", ef_thr=2.):
        super(EFDEtaFex, self).__init__(name)
        validation = TrigEFJetDEtaFexValidationMonitoring()
        online = TrigEFJetDEtaFexOnlineMonitoring()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFJetDEtaFex_Time")
        self.AthenaMonTools = [time, validation, online]
        self.DEtaCut=ef_thr

        
