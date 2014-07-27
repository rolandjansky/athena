# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigL2DEtaHypo, TrigEFDEtaHypo
from TrigJetHypo.TrigJetDEtaHypoMonitoring import TrigL2JetDEtaHypoValidationMonitoring, TrigL2JetDEtaHypoOnlineMonitoring
from TrigJetHypo.TrigJetDEtaHypoMonitoring import TrigEFJetDEtaHypoValidationMonitoring, TrigEFJetDEtaHypoOnlineMonitoring

class L2DEtaHypo(TrigL2DEtaHypo):
    __slots__ = []
    def __init__(self, name = "L2DEtaHypo", l2_thr=1):
        super(L2DEtaHypo, self).__init__( name )
        self.NGaps=l2_thr
        validation = TrigL2JetDEtaHypoValidationMonitoring()
        online = TrigL2JetDEtaHypoOnlineMonitoring()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2JetDEtaHypo_Time")
        self.AthenaMonTools = [time, validation, online]

class EFDEtaHypo(TrigEFDEtaHypo):
    __slots__ = []
    def __init__(self, name = "L2DEtaHypo", ef_thr=1):
        super(EFDEtaHypo, self).__init__( name )
        self.NGaps=ef_thr
        validation = TrigEFJetDEtaHypoValidationMonitoring()
        online = TrigEFJetDEtaHypoOnlineMonitoring()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("L2JetDEtaHypo_Time")
        self.AthenaMonTools = [time, validation, online]

class EFDEtaHypo_noJetCut(EFDEtaHypo):
    __slots__ = []
    def __init__(self, name = "EFDEtaHypo_noJetCut", ef_thr=1):
        super(EFDEtaHypo_noJetCut, self).__init__( name )
       
        self.NGaps=ef_thr
        self.forceAccept = True
