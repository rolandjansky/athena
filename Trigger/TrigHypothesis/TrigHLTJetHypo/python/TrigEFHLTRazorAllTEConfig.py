# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigEFHLTRazorAllTE
from TrigJetHLTHypo.TrigEFHLTRazorAllTEMonitoring import (
    TrigEFHLTRazorAllTEValidationMonitoring,
    TrigEFHLTRazorAllTEOnlineMonitoring)

from AthenaCommon.SystemOfUnits import GeV


class EFHLTRazorBase (TrigEFHLTRazorAllTE):
    __slots__ = []
    def __init__(self, name):
        super( EFHLTRazorBase, self ).__init__( name )

        validation = TrigEFHLTRazorAllTEValidationMonitoring()
        online = TrigEFHLTRazorAllTEOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFHLTRazor_Time")

        self.AthenaMonTools = [ time, validation, online ]
                

class EFHLTRazor (EFRazorBase):
    __slots__ = []
    def __init__(self, name = "EFHLTRazor", Razor_cut=350*GeV ):
        super( EFHLTRazor, self ).__init__( name )

        self.Razorcut = Razor_cut
