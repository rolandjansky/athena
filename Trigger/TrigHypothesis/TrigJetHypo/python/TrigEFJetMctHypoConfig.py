# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigEFJetMctHypo
from TrigJetHypo.TrigEFJetMctHypoMonitoring import TrigEFJetMctHypoValidationMonitoring, TrigEFJetMctHypoOnlineMonitoring

from AthenaCommon.SystemOfUnits import GeV


class EFJetMctBase (TrigEFJetMctHypo):
    __slots__ = []
    def __init__(self, name):
        super( EFJetMctBase, self ).__init__( name )

        validation = TrigEFJetMctHypoValidationMonitoring()
        online = TrigEFJetMctHypoOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFJetMct_Time")

        self.AthenaMonTools = [ time, validation, online ]
        

        

class EFJetLeadingMct100 (EFJetMctBase):
    __slots__ = []
    def __init__(self, name = "EFJetLeadingMct100", mct_cut=100*GeV ):
        super( EFJetLeadingMct100, self ).__init__( name )

        self.MctCut = mct_cut
        # Mulitplicitycut = 1 by default

class EFJetLeadingMct150 (EFJetMctBase):
    __slots__ = []
    def __init__(self, name = "EFJetLeadingMct150", mct_cut=150*GeV ):
        super( EFJetLeadingMct150, self ).__init__( name )

        self.MctCut = mct_cut
        # Mulitplicitycut = 1 by default

class EFJetAnyMct150 (EFJetMctBase):
    __slots__ = []
    def __init__(self, name = "EFJetAnyMct150", mct_cut=150*GeV, multiplicitycut=-1 ):
        super( EFJetAnyMct150, self ).__init__( name )

        self.MctCut = mct_cut
        self.Multiplicitycut = multiplicitycut



class EFJetLeadingMct175 (EFJetMctBase):
    __slots__ = []
    def __init__(self, name = "EFJetLeadingMct175", mct_cut=175*GeV ):
        super( EFJetLeadingMct175, self ).__init__( name )

        self.MctCut = mct_cut
        # Mulitplicitycut = 1 by default

class EFJetAnyMct175 (EFJetMctBase):
    __slots__ = []
    def __init__(self, name = "EFJetAnyMct175", mct_cut=175*GeV, multiplicitycut=-1 ):
        super( EFJetAnyMct175, self ).__init__( name )

        self.MctCut = mct_cut
        self.Multiplicitycut = multiplicitycut

