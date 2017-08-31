# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################
# L2 Electron Fex Algorithm Configuration:
# Phillip Urquijo <Phillip.Urquijo@cern.ch>
##############################

from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2ElectronFexMT 
from AthenaCommon.SystemOfUnits import GeV, mm

from AthenaCommon.AppMgr import ToolSvc

# ---------------------------------------------------------------
# class for common setups (like monitoring)
class L2ElectronFexBase(TrigL2ElectronFexMT):
    __slots__ = []
    def __init__(self, name):
        super(L2ElectronFexBase,self).__init__(name)
#        from TrigEgammaHypo.TrigL2ElectronHypoMonitoring import TrigL2ElectronFexValidationMonitoring, TrigL2ElectronFexOnlineMonitoring
#        validation = TrigL2ElectronFexValidationMonitoring()
#        online     = TrigL2ElectronFexOnlineMonitoring()
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")

#        self.AthenaMonTools = [ time, validation, online ]

        # Tracking cuts
        self.TrackPt = 1.0 * GeV
        self.TrackPtHighEt = 2.0 * GeV
        self.ClusEt = 20.0 * GeV
        self.CaloTrackdEtaNoExtrap = 0.5
        self.CaloTrackdEtaNoExtrapHighEt = 0.1
        # Calo-Tracking matching cuts
        self.CaloTrackdETA = 0.5
        self.CaloTrackdPHI = 0.5
        self.CaloTrackdEoverPLow  = 0.0
        self.CaloTrackdEoverPHigh = 999.0

# ---------------------------------------------------------------
# TrigL2ElectronFex configurations
# ---------------------------------------------------------------

class L2ElectronFex_all(L2ElectronFexBase):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_all"):
        super(L2ElectronFex_all, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # Co-ordinates of calorimeter face for extrapolation 
        self.RCalBarrelFace = 1470.0*mm
        self.ZCalEndcapFace = 3800.0*mm

class L2ElectronFex_1(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_1"):
        super(L2ElectronFex_1, self).__init__(name)
        self.AcceptAll = True
        
class L2ElectronFex_Clean(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_Clean"):
        super(L2ElectronFex_Clean, self).__init__(name)
        self.AcceptAll = False
        self.CaloTrackdETA = 0.2
        self.CaloTrackdPHI = 0.3

class L2ElectronFex_LowPt(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_LowPt"):
        super(L2ElectronFex_1, self).__init__(name)
        self.AcceptAll = False

class L2ElectronFex_HighPt(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_HighPt"):
        super(L2ElectronFex_1, self).__init__(name)
        self.AcceptAll = False
        self.TrackPt = 2.0 * GeV

