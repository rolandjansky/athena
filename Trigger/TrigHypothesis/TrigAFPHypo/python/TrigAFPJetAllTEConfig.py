# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import TrigAFPHypoConf
from TrigAFPHypo.TrigAFPJetAllTEMonitoring import (
    TrigAFPJetAllTEValidationMonitoring,
    TrigAFPJetAllTEOnlineMonitoring)

class TrigAFPJetAllTE(TrigAFPHypoConf.TrigAFPJetAllTE):
    __slots__ = []
    def __init__(self, name = "TrigAFPJetAllTE"):

        super( TrigAFPJetAllTE, self ).__init__( name )

        validation = TrigAFPJetAllTEValidationMonitoring()
        online = TrigAFPJetAllTEOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("AFPJet_Time")

        self.AthenaMonTools = [ time, validation, online ]
        self.protonTransportParamFileName1="2017_beta0p4_xAngle170_beam1.txt"
        self.protonTransportParamFileName2="2017_beta0p4_xAngle170_beam2.txt"
        self.alignmentCorrection_nearA=2.361
        self.alignmentCorrection_nearC=2.172
