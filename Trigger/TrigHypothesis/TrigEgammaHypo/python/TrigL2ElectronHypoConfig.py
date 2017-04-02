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
        self.CaloTrackdEoverPLow  = 0.0
        self.CaloTrackdEoverPHigh = 999.0
        self.TRTRatio = -999.
        
        
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

# Configurable L2 Electron class via menu
# Using cuts based on Et threshold
# Only depends on threshold 
class L2ElectronHypo_e_ID (L2ElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( L2ElectronHypo_e_ID, self ).__init__( name ) 
        self.AcceptAll = False
        # cuts
        self.TrackPt = 1.0 * GeV
        self.CaloTrackdETA = 0.2 
        self.CaloTrackdPHI = 0.3
        if(float(threshold) < 15):
            self.TrackPt = 1.0 * GeV
        elif (float(threshold) >= 15 and float(threshold) < 20):
            self.TrackPt = 2.0 * GeV
        elif (float(threshold) >= 20 and float(threshold) < 50):
            self.TrackPt = 3.0 * GeV
        elif (float(threshold) >= 50):
            self.TrackPt = 5.0 * GeV
            self.CaloTrackdETA = 999.
            self.CaloTrackdPHI = 999.
        else:
            raise RuntimeError('No threshold: Default cut configured')

# ---------------------------------------------------------------
# NoCut

class L2ElectronHypo_1(L2ElectronHypoBase):
    __slots__ = []
    def __init__(self, name="L2ElectronHypo_1"):
        super( L2ElectronHypo_1, self).__init__(name)

        # AcceptTrigIDCaloHypoAll flag: if true take events regardless of cuts
        self.AcceptAll = True
        # cuts
        self.TrackPt = 2.0 * GeV
        self.CaloTrackdETA = 0.1
        self.CaloTrackdPHI = 999.

        
