# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigTauHypo.TrigTauHypoConf import T2TauTauCombFexAlgo, T2TauTauCombHypo
from AthenaCommon.SystemOfUnits import GeV
from TrigTauHypo.TrigTauHypoMonitoring import *

class T2TauTauCombFex ( T2TauTauCombFexAlgo ):
    __slots__ = []
    def __init__(self, name="T2TauTauCombFex"):
        super(T2TauTauCombFex, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        # cuts
        #self.ValidElectron=True
        
        # require invariant mass within window and angular distance
        self.MaxDRCut=20.0
        self.MaxDPhiCut=10.0
        self.MaxDEtaCut=10.0
        self.LowerMassCut=0.0*GeV
        self.UpperMassCut=10000.0*GeV
        self.AthenaMonTools = [ T2TauTauCombValidationMonitoring(),T2TauTauCombOnlineMonitoring(), ] 
        
class T2TauTauCombHypo_loose (T2TauTauCombHypo ):  # loose selection
    __slots__ = []
    def __init__(self, name="T2TauTauCombHypo_loose"):
        super(T2TauTauCombHypo_loose, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        self.MaxDRCut=10.0
        self.MinDRCut=0.0
        self.MaxDPhiCut=10.5
        self.MaxDEtaCut=2.5
        self.LowerMassCut=1.0*GeV
        self.UpperMassCut=10000.0*GeV
        self.AthenaMonTools = [ T2TauTauCombValidationMonitoring(),T2TauTauCombOnlineMonitoring(), ] 

class T2TauTauCombHypo_medium (T2TauTauCombHypo_loose ):  # loose selection
    __slots__ = []
    def __init__(self, name="T2TauTauCombHypo_medium"):
        super(T2TauTauCombHypo_medium, self).__init__(name)
        self.MaxDEtaCut=2.0

class T2TauTauCombHypo_tight (T2TauTauCombHypo_loose ):  # loose selection
    __slots__ = []
    def __init__(self, name="T2TauTauCombHypo_tight"):
        super(T2TauTauCombHypo_tight, self).__init__(name)
        self.MaxDEtaCut=1.8

class T2TauTauCombHypo_tau29tau20 ( T2TauTauCombHypo_loose ):
    __slots__ = []
    def __init__(self, name="T2TauTauCombHypo_tau29tau20"):
        super(T2TauTauCombHypo_tau29tau20, self).__init__(name)

class T2TauTauCombHypo_tau27tau18 ( T2TauTauCombHypo_loose ):
    __slots__ = []
    def __init__(self, name="T2TauTauCombHypo_tau27tau18"):
        super(T2TauTauCombHypo_tau27tau18, self).__init__(name)

class T2TauTauCombHypo_tau27tau18_medium ( T2TauTauCombHypo_medium ):
    __slots__ = []
    def __init__(self, name="T2TauTauCombHypo_tau27tau18_medium"):
        super(T2TauTauCombHypo_tau27tau18_medium, self).__init__(name)
