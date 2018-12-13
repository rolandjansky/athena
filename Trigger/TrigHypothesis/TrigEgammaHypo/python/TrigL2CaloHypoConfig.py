# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################
# L2 Electron and Photon Calorimeter Hypothesis Algorithm Configuration:
# Phillip Urquijo <Phillip.Urquijo@cern.ch>
##############################

from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypo, TrigL2CaloLayersHypo
from AthenaCommon.SystemOfUnits import GeV

from TriggerJobOpts.TriggerFlags import TriggerFlags
menu_name = TriggerFlags.triggerMenuSetup()

from TrigL2CaloHypoCutDefs import L2CaloCutMaps
class TrigL2CaloHypoBase (TrigL2CaloHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigL2CaloHypoBase, self ).__init__( name )

        from TrigEgammaHypo.TrigL2CaloHypoMonitoring import TrigL2CaloHypoValidationMonitoring, TrigL2CaloHypoOnlineMonitoring
        validation = TrigL2CaloHypoValidationMonitoring()        
        online     = TrigL2CaloHypoOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online ]

        self.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
        self.dETACLUSTERthr = 0.1
        self.dPHICLUSTERthr = 0.1
        self.F1thr          = [0.005]
        self.ET2thr         = [90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV]
        self.HADET2thr      = [999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0]
        self.HADETthr       = [0.058, 0.058, 0.058, 0.058, 0.058, 0.058, 0.058, 0.058, 0.058]
        self.WETA2thr       = [99999.,99999.,99999.,99999.,99999.,99999.,99999.,99999.,99999.]
        self.WSTOTthr       = [99999.,99999.,99999.,99999.,99999.,99999.,99999.,99999.,99999.]
        self.F3thr          = [99999.,99999.,99999.,99999.,99999.,99999.,99999.,99999.,99999.]

        
# basic cut
class L2CaloHypo_1 (TrigL2CaloHypoBase):
    __slots__ = []
    def __init__(self, name = "L2CaloHypo_1"):
        super( L2CaloHypo_1, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Calo cuts
        self.ETthr          = [21.*GeV, 21.*GeV, 21.*GeV, 21.*GeV, 21.*GeV, 21.*GeV, 21.*GeV, 21.*GeV, 21.*GeV]
        self.CARCOREthr     = [0.92, 0.91, 0.91, 0.91, 0.93, 0.92, 0.92, 0.92, 0.92]
        self.CAERATIOthr    = [0.60, 0.60, 0.60, 0.60, 0.60, 0.60, 0.60, 0.60, 0.60]


#-------------------------------------------------------------------
# Electron Signature Configurations
#-------------------------------------------------------------------
   
#------------------------------------------------------------------
#DC 14 items
#------------------------------------------------------------------
# Fully configurable classes via TM 
# NoCut at L2 Calo used for EtCut chains
class L2CaloHypo_NoCut (TrigL2CaloHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( L2CaloHypo_NoCut, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True
        # Following just set properties but no cut applied
        self.ETthr          = [float(threshold)*GeV]*9
        self.dETACLUSTERthr = 9999.
        self.dPHICLUSTERthr = 9999.
        self.F1thr          = [0.0]
        self.HADETthr       = [9999., 9999., 9999., 9999., 9999., 9999., 9999., 9999., 9999.]
        self.CARCOREthr     = [-9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999.]
        self.CAERATIOthr    = [-9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999.]

# used in W T&P - etcut at L2
# previously named etcutTrk
class L2CaloHypo_EtCut (TrigL2CaloHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( L2CaloHypo_EtCut, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

	#L2 Threshold moved to 3 GeV within HLT threshold 
        self.ETthr          = [(float(threshold) -  3)*GeV]*9
        # No other cuts applied
        self.dETACLUSTERthr = 9999.
        self.dPHICLUSTERthr = 9999.
        self.F1thr          = [0.0]
        self.HADETthr       = [9999., 9999., 9999., 9999., 9999., 9999., 9999., 9999., 9999.]
        self.CARCOREthr     = [-9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999.]
        self.CAERATIOthr    = [-9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999.]

# Current cuts not correct! 
# Requires cut maps as function of Et and eta
# Currently using cuts from e24 items
class L2CaloHypo_e_ID (TrigL2CaloHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( L2CaloHypo_e_ID, self ).__init__( name ) 
        self.AcceptAll = False
 
        self.dETACLUSTERthr = 0.1
        self.dPHICLUSTERthr = 0.1

        # 3 GeV within HLT threshold 
        self.ETthr          = [(float(threshold) - 3)*GeV]*9
        self.HADETthr = L2CaloCutMaps(threshold).MapsHADETthr[IDinfo]
        self.CARCOREthr = L2CaloCutMaps(threshold).MapsCARCOREthr[IDinfo]
        self.CAERATIOthr =L2CaloCutMaps(threshold).MapsCAERATIOthr[IDinfo]

#-------------------------------------------------------------------
# Photon Signature Configurations
#-------------------------------------------------------------------
# the photon's calorimeter selection criteria are applied in TrigL2PhotonHypo,
# not in TrigL2CaloHypo (like the electron's)

#-------------------------------------------------------------------
# g4

class L2CaloHypo_g_nocut (TrigL2CaloHypoBase):
    __slots__ = []
    def __init__(self, name = "L2CaloHypo_g_nocut"):
        super( L2CaloHypo_g_nocut, self ).__init__( name )
        self.AcceptAll = True

        # L2 Calo cuts:
        self.ETthr          = [0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV]
        self.ET2thr         = [90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV]
        self.HADETthr       = [999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV]
        self.HADET2thr      = [999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV]
        self.CARCOREthr     = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.CAERATIOthr    = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.dETACLUSTERthr = 0.1
        self.dPHICLUSTERthr = 0.1
        self.F1thr          = [0.0]

class L2CaloHypo_g3 (TrigL2CaloHypoBase):
    __slots__ = []
    def __init__(self, name = "L2CaloHypo_g3"):
        super( L2CaloHypo_g3, self ).__init__( name )
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        
        # L2 Calo cuts: g3
        self.ETthr          = [3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV, 3.*GeV]
        self.ET2thr         = [90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV]
        self.HADETthr       = [999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV]
        self.HADET2thr      = [999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV]
        self.CARCOREthr     = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.CAERATIOthr    = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.dETACLUSTERthr = 0.1
        self.dPHICLUSTERthr = 0.1
        self.F1thr          = [0.0]
        
class L2CaloHypo_g4 (TrigL2CaloHypoBase):
    __slots__ = []
    def __init__(self, name = "L2CaloHypo_g4"):
        super( L2CaloHypo_g4, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Calo cuts: g4
        self.ETthr          = [4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV, 4.*GeV]
        self.ET2thr         = [90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV]
        self.HADETthr       = [999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV]
        self.HADET2thr      = [999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV]
        self.CARCOREthr     = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.CAERATIOthr    = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.dETACLUSTERthr = 0.1
        self.dPHICLUSTERthr = 0.1
        self.F1thr          = [0.0]

#-------------------------------------------------------------------
# g7

class L2CaloHypo_g7 (TrigL2CaloHypoBase):
    __slots__ = []
    def __init__(self, name = "L2CaloHypo_g7"):
        super( L2CaloHypo_g7, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Calo cuts: g7
        self.ETthr          = [7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV, 7.*GeV]
        self.ET2thr         = [90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV]
        self.HADETthr       = [999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV]
        self.HADET2thr      = [999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV,999.0*GeV]
        self.CARCOREthr     = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.CAERATIOthr    = [0., 0., 0., 0., 0., 0., 0., 0., 0.]
        self.dETACLUSTERthr = 0.1
        self.dPHICLUSTERthr = 0.1
        self.F1thr          = [0.0]

#-------------------------------------------------------------------
# LAr Presampler Noise detection (loose cut)

class TrigL2CaloLayersHypo_PreS_092 (TrigL2CaloLayersHypo):
    __slots__ = []
    def __init__(self, name="TrigL2CaloLayersHypo_PreS_092"):
        super( TrigL2CaloLayersHypo_PreS_092, self ).__init__( name )

        from TrigEgammaHypo.TrigL2CaloHypoMonitoring import TrigL2CaloLayersHypoValidationMonitoring, TrigL2CaloLayersHypoOnlineMonitoring
        validation = TrigL2CaloLayersHypoValidationMonitoring()        
        online     = TrigL2CaloLayersHypoOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online ]

        self.EnergyFractionCut=[0.92,1.0,1.0,1.0]
        self.AcceptAll = False

#-------------------------------------------------------------------
# LAr Presampler Noise detection (tight cut)

class TrigL2CaloLayersHypo_PreS_080 (TrigL2CaloLayersHypo):
    __slots__ = []
    def __init__(self, name="TrigL2CaloLayersHypo_PreS_080"):
        super( TrigL2CaloLayersHypo_PreS_080, self ).__init__( name )

        from TrigEgammaHypo.TrigL2CaloHypoMonitoring import TrigL2CaloLayersHypoValidationMonitoring, TrigL2CaloLayersHypoOnlineMonitoring
        validation = TrigL2CaloLayersHypoValidationMonitoring()        
        online     = TrigL2CaloLayersHypoOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online ]

        self.EnergyFractionCut=[0.80,1.0,1.0,1.0]
        self.AcceptAll = False

