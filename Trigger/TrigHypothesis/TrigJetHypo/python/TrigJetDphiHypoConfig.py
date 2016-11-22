# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from math import pi
from TrigJetHypo.TrigJetHypoConf import TrigL2JetDphiHypo
#from TrigJetHypo.TrigJetHypoConf import TrigEFJetDphiHypo

from AthenaCommon.SystemOfUnits import GeV

class L2JetDphiHypoBase (TrigL2JetDphiHypo):
    __slots__ = []
    def __init__(self, name):
        super( L2JetDphiHypoBase, self ).__init__( name )
        
        from TrigJetHypo.TrigJetDphiHypoMonitoring import TrigL2JetDphiHypoValidationMonitoring, TrigL2JetDphiHypoOnlineMonitoring
        validation = TrigL2JetDphiHypoValidationMonitoring()
        online = TrigL2JetDphiHypoOnlineMonitoring()
               
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TrigL2JetDphiHypo_Time")
        
        self.AthenaMonTools = [ time, validation, online ]
        
#class EFJetDphiHypoBase (TrigEFJetDphiHypo):
#    __slots__ = []
#    def __init__(self, name):
#        super( EFJetDphiHypoBase, self ).__init__( name )
#
#        from TrigJetHypo.TrigJetDphiHypoMonitoring import TrigEFJetDphiHypoValidationMonitoring, TrigEFJetDphiHypoOnlineMonitoring, TrigEFJetDphiHypoCosmicMonitoring
#        validation = TrigEFJetDphiHypoValidationMonitoring()
#        online = TrigEFJetDphiHypoOnlineMonitoring()
#        cosmic = TrigEFJetDphiHypoCosmicMonitoring()
#
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("EFJetDphiHypo_Time")
#
#        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
class L2JetDphiHypo (L2JetDphiHypoBase):
    __slots__ = []
    def __init__(self, name = "L2JetDphiHypo", dphimax=pi-0.4, pthard=20*GeV,ptsoft=15*GeV):
        super( L2JetDphiHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        #self.AcceptAll=False

        # cuts
        self.JetDphiMinCut = 0.4
        self.JetDphiMaxCut = dphimax
        #self.ptHardJetCut = 150*GeV
        #self.ptSoftJetCut =  35*GeV
        self.ptHardJetCut = pthard
        self.ptSoftJetCut = ptsoft

#class EFJetDphiHypo (EFJetDphiHypoBase):
#    __slots__ = []
#    def __init__(self, name = "EFJetDphiHypo",ef_thr=20*GeV):
#        super( EFJetDphiHypo, self ).__init__( name )
#
#        self.Etcut = ef_thr
