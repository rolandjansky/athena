# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigL2CentralJetAllTE
from TrigJetHypo.TrigL2CentralJetAllTEMonitoring import TrigL2CentralJetAllTEValidationMonitoring, TrigL2CentralJetAllTEOnlineMonitoring 

class TrigL2CentralJetAllTE(TrigL2CentralJetAllTE):
    __slots__ = []
    def __init__(self, name = "TrigL2CentralJetAllTE"):
        super(TrigL2CentralJetAllTE, self).__init__( name )
        validation = TrigL2CentralJetAllTEValidationMonitoring()
        online = TrigL2CentralJetAllTEOnlineMonitoring()
        self.AthenaMonTools = [ validation, online]

