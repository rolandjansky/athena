# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigEFRazorAllTE
from TrigHLTJetHypo.TrigEFRazorAllTEMonitoring import (
    TrigEFRazorAllTEValidationMonitoring,
    TrigEFRazorAllTEOnlineMonitoring)

from AthenaCommon.SystemOfUnits import GeV


class EFRazorBase (TrigEFRazorAllTE):
    __slots__ = []
    def __init__(self, name):
        super( EFRazorBase, self ).__init__( name )

        validation = TrigEFRazorAllTEValidationMonitoring()
        online = TrigEFRazorAllTEOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFRazor_Time")

        self.AthenaMonTools = [ time, validation, online ]
                

class EFRazor (EFRazorBase):
    __slots__ = []
    def __init__(self, name = "EFRazor", Razor_cut=350*GeV ):
        super( EFRazor, self ).__init__( name )

        self.Razorcut = Razor_cut
