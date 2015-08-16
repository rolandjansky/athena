# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from math import pi
from TrigJetHypo.TrigJetHypoConf import TrigL2JetMctHypo
#from TrigJetHypo.TrigJetHypoConf import TrigEFJetMctHypo

from AthenaCommon.SystemOfUnits import GeV

class L2JetMctHypoBase (TrigL2JetMctHypo):
    __slots__ = []
    def __init__(self, name):
        super( L2JetMctHypoBase, self ).__init__( name )
        
        from TrigJetHypo.TrigJetMctHypoMonitoring import TrigL2JetMctHypoValidationMonitoring, TrigL2JetMctHypoOnlineMonitoring
        validation = TrigL2JetMctHypoValidationMonitoring()
        online = TrigL2JetMctHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TrigL2JetMctHypo_Time")
        
        self.AthenaMonTools = [ time, validation, online ]
        
#class EFJetMctHypoBase (TrigEFJetMctHypo):
#    __slots__ = []
#    def __init__(self, name):
#        super( EFJetMctHypoBase, self ).__init__( name )
#
#        from TrigJetHypo.TrigJetMctHypoMonitoring import TrigEFJetMctHypoValidationMonitoring, TrigEFJetMctHypoOnlineMonitoring, TrigEFJetMctHypoCosmicMonitoring
#        validation = TrigEFJetMctHypoValidationMonitoring()
#        online = TrigEFJetMctHypoOnlineMonitoring()
#        cosmic = TrigEFJetMctHypoCosmicMonitoring()
#
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("EFJetMctHypo_Time")
#
#        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
class L2JetMctHypo (L2JetMctHypoBase):
    __slots__ = []
    def __init__(self, name = "L2JetMctHypo", mct=150*GeV, pthard=20*GeV,ptsoft=15*GeV):
        super( L2JetMctHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        #self.AcceptAll=False

        # cuts
        self.JetMctCut = mct
        #self.ptHardJetCut = 150*GeV
        #self.ptSoftJetCut =  35*GeV
        self.ptHardJetCut = pthard
        self.ptSoftJetCut = ptsoft

#class EFJetMctHypo (EFJetMctHypoBase):
#    __slots__ = []
#    def __init__(self, name = "EFJetMctHypo",ef_thr=20*GeV):
#        super( EFJetMctHypo, self ).__init__( name )
#
#        self.Etcut = ef_thr
