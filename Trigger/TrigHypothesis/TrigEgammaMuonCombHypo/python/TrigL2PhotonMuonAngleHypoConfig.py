# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigEgammaMuonCombHypo.TrigEgammaMuonCombHypoConf import TrigL2PhotonMuonAngleFexAlgo, TrigL2ElectronMuonAngleHypo
from AthenaCommon.SystemOfUnits import GeV
import TrigEgammaMuonCombHypo.TrigL2ElectronMuonAngleHypoMonitoring as mon

class TrigL2PhotonMuonAngleFex ( TrigL2PhotonMuonAngleFexAlgo ):
    __slots__ = []
    def __init__(self, name="TrigL2PhotonMuonAngleFex"):
        super(TrigL2PhotonMuonAngleFex, self).__init__(name)
        
        self.AthenaMonTools = [ mon.TrigL2PhotonMuonAngleOnlineMonitoring(), mon.TrigL2PhotonMuonAngleValidationMonitoring() ]

# new cuts as presented at TGM on April 20
class TrigL2PhotonMuonAngleHypo_tau (TrigL2ElectronMuonAngleHypo ):  # tight selection
    __slots__ = []
    def __init__(self, name="TrigL2PhotonMuonAngleHypo_tau"):
        super(TrigL2PhotonMuonAngleHypo_tau, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        self.inputLabel="L2_PhotonMuonTopoFEX"
        
        # require invariant mass within window, distance within a range plus maximal separation in R-phi plane.
        self.OppositeSign=False
        self.CommonVertex=False
        
        self.MaxDRCut=2.
        self.MinDRCut=0.0
       
        self.MaxDPhiCut=2.
        
        
        self.LowerMassCut=0.*GeV
        self.UpperMassCut=6.0*GeV
        
        self.AthenaMonTools = [ mon.TrigL2ElectronMuonAngleOnlineMonitoring(), mon.TrigL2ElectronMuonAngleValidationMonitoring_emutopo() ]
