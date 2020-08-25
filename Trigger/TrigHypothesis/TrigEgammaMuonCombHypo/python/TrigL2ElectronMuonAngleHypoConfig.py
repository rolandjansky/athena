# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigEgammaMuonCombHypo.TrigEgammaMuonCombHypoConf import TrigL2ElectronMuonAngleFexAlgo, TrigL2ElectronMuonAngleHypo
from AthenaCommon.SystemOfUnits import GeV
import TrigEgammaMuonCombHypo.TrigL2ElectronMuonAngleHypoMonitoring as mon

class TrigL2ElectronMuonAngleFex ( TrigL2ElectronMuonAngleFexAlgo ):
    __slots__ = []
    def __init__(self, name="TrigL2ElectronMuonAngleFex"):
        super(TrigL2ElectronMuonAngleFex, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        # cuts
        self.ValidElectron=True
        
        # require invariant mass within window and angular distance
        self.MaxDRCut=20.0
        self.MaxDPhiCut=4.0
        self.LowerMassCut=0.0*GeV
        self.UpperMassCut=999.0*GeV

        self.AthenaMonTools = [ mon.TrigL2ElectronMuonAngleOnlineMonitoring(), mon.TrigL2ElectronMuonAngleValidationMonitoring_emutopo() ]

# new cuts as presented at TGM on April 20
class TrigL2ElectronMuonAngleHypo_tight (TrigL2ElectronMuonAngleHypo ):  # tight selection
    __slots__ = []
    def __init__(self, name="TrigL2ElectronMuonAngleHypo_tight"):
        super(TrigL2ElectronMuonAngleHypo_tight, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        
        # require invariant mass within window, distance within a range plus maximal separation in R-phi plane.
        self.OppositeSign=True
        self.CommonVertex=True
        
        self.MaxDRCut=3.0
        self.MinDRCut=0.0
       
        self.MaxDPhiCut=1.5
        
        
        self.LowerMassCut=1.5*GeV
        self.UpperMassCut=10.0*GeV
        
        self.AthenaMonTools = [ mon.TrigL2ElectronMuonAngleOnlineMonitoring(), mon.TrigL2ElectronMuonAngleValidationMonitoring_emutopo() ]
        
class TrigL2ElectronMuonAngleHypo_medium (TrigL2ElectronMuonAngleHypo ):  # medium selection
    __slots__ = []
    def __init__(self, name="TrigL2ElectronMuonAngleHypo_medium"):
        super(TrigL2ElectronMuonAngleHypo_medium, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        
        # require invariant mass within window, distance within a range plus maximal separation in R-phi plane.
        self.OppositeSign=False
        self.CommonVertex=False
        
        self.MaxDRCut=3.0
        self.MinDRCut=0.0
       
        self.MaxDPhiCut=1.5
        
        
        self.LowerMassCut=1.5*GeV
        self.UpperMassCut=10.0*GeV
        
        self.AthenaMonTools = [ mon.TrigL2ElectronMuonAngleOnlineMonitoring(), mon.TrigL2ElectronMuonAngleValidationMonitoring_emutopo() ]

class TrigL2ElectronMuonAngleHypo_loose (TrigL2ElectronMuonAngleHypo ):  # loose selection
    __slots__ = []
    def __init__(self, name="TrigL2ElectronMuonAngleHypo_loose"):
        super(TrigL2ElectronMuonAngleHypo_loose, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        
        # require invariant mass within window, distance within a range plus maximal separation in R-phi plane.
        self.OppositeSign=False
        self.CommonVertex=False
        
        self.MaxDRCut=4.0
        self.MinDRCut=0.0
       
        self.MaxDPhiCut=1.5
        
        
        self.LowerMassCut=1.0*GeV
        self.UpperMassCut=20.0*GeV
        
        self.AthenaMonTools = [ mon.TrigL2ElectronMuonAngleOnlineMonitoring(), mon.TrigL2ElectronMuonAngleValidationMonitoring_emutopo() ]

#algorithms configured in TriggerMenuPython - to prevent athena crash in the transition period  

        
class TrigL2ElectronMuonAngleHypo_e5mu4 (TrigL2ElectronMuonAngleHypo ):  # tight selection
    __slots__ = []
    def __init__(self, name="TrigL2ElectronMuonAngleHypo_e5mu4"):
        super(TrigL2ElectronMuonAngleHypo_e5mu4, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        
        # require invariant mass within window, distance within a range plus maximal separation in R-phi plane.
        self.OppositeSign=True
        self.CommonVertex=True
        
        self.MaxDRCut=3.0
        self.MinDRCut=0.0
       
        self.MaxDPhiCut=1.5
        
        
        self.LowerMassCut=1.5*GeV
        self.UpperMassCut=10.0*GeV
        
        self.AthenaMonTools = [ mon.TrigL2ElectronMuonAngleOnlineMonitoring(), mon.TrigL2ElectronMuonAngleValidationMonitoring_emutopo() ]
        
class TrigL2ElectronMuonAngleHypo_e5mu4_medium (TrigL2ElectronMuonAngleHypo ):  # medium selection
    __slots__ = []
    def __init__(self, name="TrigL2ElectronMuonAngleHypo_e5mu4_medium"):
        super(TrigL2ElectronMuonAngleHypo_e5mu4_medium, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        
        # require invariant mass within window, distance within a range plus maximal separation in R-phi plane.
        self.OppositeSign=False
        self.CommonVertex=False
        
        self.MaxDRCut=3.0
        self.MinDRCut=0.0
       
        self.MaxDPhiCut=1.5
        
        
        self.LowerMassCut=1.5*GeV
        self.UpperMassCut=10.0*GeV
        
        self.AthenaMonTools = [ mon.TrigL2ElectronMuonAngleOnlineMonitoring(), mon.TrigL2ElectronMuonAngleValidationMonitoring_emutopo() ]
