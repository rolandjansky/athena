# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigTauHypo.TrigTauHypoConf import EFTauTauCombFexAlgo, EFTauTauCombHypo
from AthenaCommon.SystemOfUnits import GeV
from TrigTauHypo.TrigTauHypoMonitoring import *


class EFTauTauCombFex ( EFTauTauCombFexAlgo ):
    __slots__ = []
    def __init__(self, name="EFTauTauCombFex"):
        super(EFTauTauCombFex, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
	
        # require invariant mass within window
        self.LowerMassCut=0.0*GeV
        self.UpperMassCut=10000.0*GeV
        #self.UseRecommended=True
        self.MaxDRCut=99.0
        self.MaxDPhiCut=99.0
        self.MaxDEtaCut=99.0
        self.AthenaMonTools = [ EFTauTauCombValidationMonitoring(), EFTauTauCombOnlineMonitoring(), ]
        
class EFTauTauCombHypo_loose ( EFTauTauCombHypo ):
    __slots__ = []
    def __init__(self, name="EFTauTauCombHypo_loose"):
        super(EFTauTauCombHypo_loose, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # require invariant mass within window
        self.LowerMassCut=1.0*GeV
        self.UpperMassCut=10000.0*GeV
        self.CommonVertex=False
        self.OppositeSign=False
        self.MaxDRCut=40.0
        self.MinDRCut=0.0
        self.MaxDPhiCut=10.5
        self.MaxDEtaCut=2.5
        self.AthenaMonTools = [ EFTauTauCombValidationMonitoring(), EFTauTauCombOnlineMonitoring(), ]

class EFTauTauCombHypo_medium ( EFTauTauCombHypo_loose ):
    __slots__ = []
    def __init__(self, name="EFTauTauCombHypo_medium"):
        super(EFTauTauCombHypo_medium, self).__init__(name)
        self.MaxDEtaCut=2.0

class EFTauTauCombHypo_tight ( EFTauTauCombHypo_loose ):
    __slots__ = []
    def __init__(self, name="EFTauTauCombHypo_tight"):
        super(EFTauTauCombHypo_tight, self).__init__(name)
        self.MaxDEtaCut=1.8


class EFTauTauCombHypo_tau29tau20 ( EFTauTauCombHypo_loose ):
    __slots__ = []
    def __init__(self, name="EFTauTauCombHypo_tau29tau20"):
        super(EFTauTauCombHypo_tau29tau20, self).__init__(name)

class EFTauTauCombHypo_tau27tau18 ( EFTauTauCombHypo_loose ):
    __slots__ = []
    def __init__(self, name="EFTauTauCombHypo_tau27tau18"):
        super(EFTauTauCombHypo_tau27tau18, self).__init__(name)

class EFTauTauCombHypo_tau27tau18_medium ( EFTauTauCombHypo_medium ):
    __slots__ = []
    def __init__(self, name="EFTauTauCombHypo_tau27tau18_medium"):
        super(EFTauTauCombHypo_tau27tau18_medium, self).__init__(name)
