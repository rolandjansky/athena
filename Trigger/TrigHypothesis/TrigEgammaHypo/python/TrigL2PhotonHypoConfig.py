# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypo

from AthenaCommon.SystemOfUnits import GeV
from TrigL2CaloHypoCutDefs import L2CaloPhotonCutMaps
# basic cut
class TrigL2PhotonHypoBase (TrigL2PhotonHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigL2PhotonHypoBase, self ).__init__( name )

        from TrigEgammaHypo.TrigL2PhotonHypoMonitoring import TrigL2PhotonHypoValidationMonitoring, TrigL2PhotonHypoOnlineMonitoring, TrigL2PhotonHypoCosmicMonitoring
        validation = TrigL2PhotonHypoValidationMonitoring()
        online     = TrigL2PhotonHypoOnlineMonitoring()
        cosmic     = TrigL2PhotonHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]

        self.ETthr          = [0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV]
        self.CARCOREthr     = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.CAERATIOthr    = [0., 0., 0., 0., 0., 0., 0., 0., 0.]

        self.EtaBins        = [0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
        self.dETACLUSTERthr = 0.1
        self.dPHICLUSTERthr = 0.1
        self.F1thr          = [0.005]
        self.ET2thr         = [90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV]
        self.HADET2thr      = [999, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0]
        self.HADETthr       = [0.035, 0.035, 0.035, 0.035, 0.035, 0.035, 0.035, 0.035, 0.035]

# EtCut only chains
# Et threshold cut applied at L2 only
class L2PhotonHypo_g_EtCut (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( L2PhotonHypo_g_EtCut, self ).__init__( name ) 
        self.AcceptAll = False
        self.EtCutOnly = True
        self.ETthr          = [(float(threshold)-1.)*GeV]*9

class L2PhotonHypo_g_NoCut (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( L2PhotonHypo_g_NoCut, self ).__init__( name ) 
        self.AcceptAll = True
        self.EtCutOnly = True
        self.ETthr          = [(float(threshold)-1.)*GeV]*9

class L2PhotonHypo_g_ID (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( L2PhotonHypo_g_ID, self ).__init__( name ) 
        PhotonCuts = L2CaloPhotonCutMaps(threshold)
        self.AcceptAll = False
        self.EtCutOnly = False
        self.ETthr          = [(float(threshold)-3.)*GeV]*9
        
        self.HADETthr = PhotonCuts.MapsHADETthr[IDinfo]
        self.CARCOREthr = PhotonCuts.MapsCARCOREthr[IDinfo]
        self.CAERATIOthr = PhotonCuts.MapsCAERATIOthr[IDinfo]

#a hypo config for the g5_nocut that is seeded by the MTBS
class L2PhotonHypo_g5_nocut_mtbs (TrigL2PhotonHypoBase):
    __slots__ = []
    def __init__(self, name = "L2PhotonHypo_g5_nocut_mtbs"):
        super( L2PhotonHypo_g5_nocut_mtbs, self ).__init__( name )
        self.AcceptAll = True

