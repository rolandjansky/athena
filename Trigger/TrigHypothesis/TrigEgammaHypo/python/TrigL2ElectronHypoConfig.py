# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################
# L2 Electron  Hypothesis Algorithm Configuration:
# Phillip Urquijo <Phillip.Urquijo@cern.ch>
# Maintained by Ryan M White <ryan.white@cern.ch>
##############################

# 12-2-10
# (VD) - removing dPhi cut from every Loose/Medium chain (consistency with OFFLINE selection)
#      - removing dEt  cut from every Loose chain and loosen ( 0.2 ) for Medium ones(consistency with OFFLINE selection) 

from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2ElectronHypo
from AthenaCommon.SystemOfUnits import GeV, mm

# ---------------------------------------------------------------
# class for common setups (like monitoring)
class L2ElectronHypoBase(TrigL2ElectronHypo):
    __slots__ = []
    def __init__(self, name):
        super(L2ElectronHypoBase,self).__init__(name)
        from TrigEgammaHypo.TrigL2ElectronHypoMonitoring import TrigL2ElectronHypoValidationMonitoring, TrigL2ElectronHypoOnlineMonitoring
        validation = TrigL2ElectronHypoValidationMonitoring()
        online     = TrigL2ElectronHypoOnlineMonitoring()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online ]
        
        #common attributes
        self.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
        self.CaloTrackdEoverPLow  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        self.CaloTrackdEoverPHigh = [999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0]
        self.TRTRatio = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.EtaBinsTRT     = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
        self.CaloTrackdEoverPLowTRT  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        self.CaloTrackdEoverPHighTRT = [999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0]
        self.TRTRatioTRT = [-999., -999., -999., -999., -999., -999., -999., -999., -999.]
        self.CaloTrackdETATRT = [999., 999., 999., 999., 999., 999., 999., 999., 999.]
        
        
# ---------------------------------------------------------------
# TrigL2ElectronHypo configurations
# ---------------------------------------------------------------
#-------------------------------------------
# --- Following classes to be used with TriggerMenu
# --- Rely on TM to configure 
# --- RMWhite 2014
#
# --- Et Cut only -- threshold passed from TM chain name
class L2ElectronHypo_e_NoCut(L2ElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( L2ElectronHypo_e_NoCut, self).__init__(name)
        self.AcceptAll = True

# --- No Cut but set the track algorithm
class L2ElectronHypo_e_NoCut_TrkAlgo(L2ElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( L2ElectronHypo_e_NoCut, self).__init__(name)
        self.AcceptAll = True

# Configurable L2 Electron class via menu
# Using cuts based on Et threshold
# Need to be fixed for ID
class L2ElectronHypo_e_ID (L2ElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( L2ElectronHypo_e_ID, self ).__init__( name ) 
        self.AcceptAll = False
        self.TrackAlgoId = 9 # [0=All, 3=TRT, 5=SiTrack or IdScan, 6, strategyB, 7, strategyC, 9 new Fast Tracking]
        # cuts
        self.TrackPt = 1.0 * GeV
        self.CaloTrackdETA = [ 0.2 , 0.2 , 0.2 , 0.2 , 0.2 , 0.2 , 0.2 , 0.2 , 0.2 ]
        self.CaloTrackdPHI = [ 999., 999., 999., 999., 999., 999., 999., 999., 999.]
        if(float(threshold) < 15):
            self.TrackPt = 1.0 * GeV
        elif (float(threshold) >= 15 and float(threshold) < 20):
            self.TrackPt = 2.0 * GeV
        elif (float(threshold) >= 20 and float(threshold) < 50):
            self.TrackPt = 3.0 * GeV
        elif (float(threshold) >= 50):
            self.TrackPt = 5.0 * GeV
            self.CaloTrackdETA = [ 999., 999., 999., 999., 999., 999., 999., 999., 999.]
            self.CaloTrackdPHI = [ 999., 999., 999., 999., 999., 999., 999., 999., 999.]
        else:
            raise RuntimeError('No threshold: Default cut configured')

# Configurable L2 Track Algorithm
class L2ElectronHypo_e_ID_L2TrkAlg (L2ElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, L2IDAlg):
        super( L2ElectronHypo_e_ID_L2TrkAlg, self ).__init__( name, threshold, IDinfo ) 
        # Here we need to set the TrackAlgoID from the menu name
        if L2IDAlg == 'TRT':
            self.TrackAlgoId = 3
        elif L2IDAlg == 'L2StarA': 
            self.TrackAlgoId = 5
        elif L2IDAlg == 'L2StarB':
            self.TrackAlgoId = 6
        elif L2IDAlg == 'L2StarC':
            self.TrackAlgoId = 7
        elif L2IDAlg == 'FTK':
            self.TrackAlgoId = 8
        else:
            self.TrackAlgoId = 9 # [0=All, 3=TRT, 5=SiTrack or IdScan, 6=L2StarB, 7=L2StarC, 8=FTK, 9=Fast]

# TRT has different cuts
class L2ElectronHypo_e_ID_TRT (L2ElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( L2ElectronHypo_e_ID_TRT, self ).__init__( name, threshold, IDinfo ) 
        # Here we need to set the TrackAlgoID from the menu name
        self.TrackAlgoId = 3 # [0=All, 3=TRT, 5=SiTrack or IdScan, 6=L2StarB, 7=L2StarC]
        # cuts
        self.TrackPtTRT = 1.0 * GeV
        self.CaloTrackdPHITRT = [0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3]

# ---------------------------------------------------------------
# NoCut

class L2ElectronHypo_1(L2ElectronHypoBase):
    __slots__ = []
    def __init__(self, name="L2ElectronHypo_1"):
        super( L2ElectronHypo_1, self).__init__(name)

        # AcceptTrigIDCaloHypoAll flag: if true take events regardless of cuts
        self.AcceptAll = True
        self.TrackAlgoId = 9 # [0=All, 3=TRT, 5=SiTrack or IdScan]
        # cuts
        self.TrackPt = 2.0 * GeV
        self.CaloTrackdETA = [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1]
        self.CaloTrackdPHI = [ 999., 999., 999., 999., 999., 999., 999., 999., 999.]

        
