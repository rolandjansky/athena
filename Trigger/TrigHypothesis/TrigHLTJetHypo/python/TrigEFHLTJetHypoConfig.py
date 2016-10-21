# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigEFHLTJetHypo
from AthenaCommon.SystemOfUnits import GeV


class EFHLTJetHypoBase (TrigEFHLTJetHypo):
    __slots__ = []
    def __init__(self, name):
        super( EFHLTJetHypoBase, self ).__init__( name )

        from TrigJetHypo.TrigJetHypoMonitoring import TrigEFJetHypoValidationMonitoring, TrigEFJetHypoOnlineMonitoring, TrigEFJetHypoCosmicMonitoring
        validation = TrigEFJetHypoValidationMonitoring()
        online = TrigEFJetHypoOnlineMonitoring()
        cosmic = TrigEFJetHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFJetHypo_Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
        # switch on basic cleaning by default for all chains
        #self.doBasicCleaning = True
        self.n90CleaningThreshold = 2
        self.presamplerCleaningThreshold = 0.9
        self.negativeECleaningThreshold = -60e3    # 60 GeV
        self.qmeanCleaningThreshold = 0.8
        self.HECQCleaningThreshold = 0.5
        self.HECfCleaningThreshold = 0.5
        self.LArQCleaningThreshold = 0.8
        self.EMfCleaningThreshold = 0.95

        
class EFHLTCentFullScanMultiJetHypo (EFHLTJetHypoBase):
    __slots__ = []
    def __init__(self, name = "EFHLTCentFullScanMultiJetHypo",ef_thr=20*GeV,multiplicity=2, ef_thrs=[],etaMin=0,etaMax=3.2):
        super( EFHLTCentFullScanMultiJetHypo, self ).__init__( name )

        self.Etcut = ef_thr
        self.etaMincut = etaMin
        self.etaMaxcut = etaMax
        self.Multiplicitycut = multiplicity
        self.EtThresholds=ef_thrs

