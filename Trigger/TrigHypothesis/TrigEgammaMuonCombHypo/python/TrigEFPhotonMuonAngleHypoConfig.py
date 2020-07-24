# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigEgammaMuonCombHypo.TrigEgammaMuonCombHypoConf import TrigEFPhotonMuonAngleFexAlgo, TrigEFElectronMuonAngleHypo
from AthenaCommon.SystemOfUnits import GeV
import TrigEgammaMuonCombHypo.TrigEFElectronMuonAngleHypoMonitoring as mon

class TrigEFPhotonMuonAngleFex ( TrigEFPhotonMuonAngleFexAlgo ):
    __slots__ = []
    def __init__(self, name="TrigEFPhotonMuonAngleFex"):
        super(TrigEFPhotonMuonAngleFex, self).__init__(name)

        self.AthenaMonTools = [ mon.TrigEFPhotonMuonAngleOnlineMonitoring() ]
  
  
class TrigEFPhotonMuonAngleHypo_tau ( TrigEFElectronMuonAngleHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFPhotonMuonAngleHypo_tau"):
        super(TrigEFPhotonMuonAngleHypo_tau, self).__init__(name)

        self.inputLabel= "EF_PhotonMuonTopoFEX"
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
                
        # require invariant mass within window
        self.LowerMassCut=0.*GeV
        self.UpperMassCut=2.5*GeV
        self.CommonVertex=False
        self.OppositeSign=False
        self.MaxDRCut=0.6
        self.MinDRCut=0.
        self.MaxDPhiCut=1.5

        self.AthenaMonTools = [ mon.TrigEFElectronMuonAngleOnlineHypoMonitoring(), mon.TrigEFElectronMuonAngleValidationHypoMonitoring_emutopo() ]
