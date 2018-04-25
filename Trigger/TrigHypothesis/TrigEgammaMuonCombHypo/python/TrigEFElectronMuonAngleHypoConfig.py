# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEgammaMuonCombHypo.TrigEgammaMuonCombHypoConf import TrigEFElectronMuonAngleFexAlgo, TrigEFElectronMuonAngleHypo
from AthenaCommon.SystemOfUnits import GeV
from TrigEgammaMuonCombHypo.TrigEFElectronMuonAngleHypoMonitoring import *

class TrigEFElectronMuonAngleFex ( TrigEFElectronMuonAngleFexAlgo ):
    __slots__ = []
    def __init__(self, name="TrigEFElectronMuonAngleFex"):
        super(TrigEFElectronMuonAngleFex, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False

        # require invariant mass within window
        self.LowerMassCut=0.0*GeV
        self.UpperMassCut=999.0*GeV
        self.UseRecommended=True
        self.MaxDRCut=99.0
        self.MaxDPhiCut=99.0

        self.AthenaMonTools = [ TrigEFElectronMuonAngleOnlineFexMonitoring(), TrigEFElectronMuonAngleValidationFexMonitoring_emutopo() ] 
  
# new cuts as presented at TGM on April 20
class TrigEFElectronMuonAngleHypo_tight ( TrigEFElectronMuonAngleHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFElectronMuonAngleHypo_tight"):
        super(TrigEFElectronMuonAngleHypo_tight, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
                
        # require invariant mass within window
        self.LowerMassCut=1.5*GeV
        self.UpperMassCut=10.0*GeV
        self.CommonVertex=True
        self.OppositeSign=True
        self.MaxDRCut=3.0
        self.MinDRCut=0.1
        self.MaxDPhiCut=1.5

        
        self.AthenaMonTools = [ TrigEFElectronMuonAngleOnlineHypoMonitoring(), TrigEFElectronMuonAngleValidationHypoMonitoring_emutopo() ] 
  
class TrigEFElectronMuonAngleHypo_medium ( TrigEFElectronMuonAngleHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFElectronMuonAngleHypo_medium"):
        super(TrigEFElectronMuonAngleHypo_medium, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
                
        # require invariant mass within window
        self.LowerMassCut=1.5*GeV
        self.UpperMassCut=10.0*GeV
        self.CommonVertex=False
        self.OppositeSign=False
        self.MaxDRCut=3.0
        self.MinDRCut=0.1
        self.MaxDPhiCut=1.5

        
        self.AthenaMonTools = [ TrigEFElectronMuonAngleOnlineHypoMonitoring(), TrigEFElectronMuonAngleValidationHypoMonitoring_emutopo() ] 

class TrigEFElectronMuonAngleHypo_loose ( TrigEFElectronMuonAngleHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFElectronMuonAngleHypo_loose"):
        super(TrigEFElectronMuonAngleHypo_loose, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
                
        # require invariant mass within window
        self.LowerMassCut=1.0*GeV
        self.UpperMassCut=20.0*GeV
        self.CommonVertex=False
        self.OppositeSign=False
        self.MaxDRCut=4.0
        self.MinDRCut=0.1
        self.MaxDPhiCut=1.5

        
        self.AthenaMonTools = [ TrigEFElectronMuonAngleOnlineHypoMonitoring(), TrigEFElectronMuonAngleValidationHypoMonitoring_emutopo() ] 

#algorithms configured in TriggerMenuPython - to prevent athena crash in the transition period  
   
class TrigEFElectronMuonAngleHypo_e5mu4 ( TrigEFElectronMuonAngleHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFElectronMuonAngleHypo_e5mu4"):
        super(TrigEFElectronMuonAngleHypo_e5mu4, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
                
        # require invariant mass within window
        self.LowerMassCut=1.5*GeV
        self.UpperMassCut=10.0*GeV
        self.CommonVertex=True
        self.OppositeSign=True
        self.MaxDRCut=3.0
        self.MinDRCut=0.1
        self.MaxDPhiCut=1.5

        
        self.AthenaMonTools = [ TrigEFElectronMuonAngleOnlineHypoMonitoring(), TrigEFElectronMuonAngleValidationHypoMonitoring_emutopo() ] 

class TrigEFElectronMuonAngleHypo_e5mu4_medium ( TrigEFElectronMuonAngleHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFElectronMuonAngleHypo_e5mu4_medium"):
        super(TrigEFElectronMuonAngleHypo_e5mu4_medium, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
                
        # require invariant mass within window
        self.LowerMassCut=1.5*GeV
        self.UpperMassCut=10.0*GeV
        self.CommonVertex=False
        self.OppositeSign=False
        self.MaxDRCut=3.0
        self.MinDRCut=0.1
        self.MaxDPhiCut=1.5

        
        self.AthenaMonTools = [ TrigEFElectronMuonAngleOnlineHypoMonitoring(), TrigEFElectronMuonAngleValidationHypoMonitoring_emutopo() ] 

class TrigEFElectronMuonAngleHypo_bXemu ( TrigEFElectronMuonAngleHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFElectronMuonAngleHypo_bXemu"):
        super(TrigEFElectronMuonAngleHypo_bXemu, self).__init__(name)

        self.inputLabel= "EF_ElectronMuonTopoFEX"
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
                
        # require invariant mass within window
        self.LowerMassCut=0.1*GeV
        self.UpperMassCut=7.0*GeV
        self.CommonVertex=True
        self.OppositeSign=True
        self.MaxDRCut=3.
        self.MinDRCut=0.
        self.MaxDPhiCut=1.5

        self.AthenaMonTools = [ TrigEFElectronMuonAngleOnlineHypoMonitoring(), TrigEFElectronMuonAngleValidationHypoMonitoring_emutopo() ] 
