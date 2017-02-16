# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################
# L2 Electron Fex Algorithm Configuration:
# Phillip Urquijo <Phillip.Urquijo@cern.ch>
##############################

from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2ElectronFex
from AthenaCommon.SystemOfUnits import GeV, mm

from AthenaCommon.AppMgr import ToolSvc
#from TrackToCalo.ExtrapolateToCaloToolBase import \
#     ExtrapolateToCaloToolFactory
#extrap = ExtrapolateToCaloToolFactory (depth='showerdefault',
#                                       straightLine=False)
#ToolSvc+=extrap

# ---------------------------------------------------------------
# class for common setups (like monitoring)
class L2ElectronFexBase(TrigL2ElectronFex):
    __slots__ = []
    def __init__(self, name):
        super(L2ElectronFexBase,self).__init__(name)
        from TrigEgammaHypo.TrigL2ElectronHypoMonitoring import TrigL2ElectronFexValidationMonitoring, TrigL2ElectronFexOnlineMonitoring
        validation = TrigL2ElectronFexValidationMonitoring()
        online     = TrigL2ElectronFexOnlineMonitoring()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online ]

        self.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
        # Tracking cuts
        self.TrackPt = 1.0 * GeV
        # Calo-Tracking matching cuts
        self.CaloTrackdETA = [0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]
        self.CaloTrackdPHI = [0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]
        self.CaloTrackdEoverPLow  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        self.CaloTrackdEoverPHigh = [999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0]
        self.dETACLUSTERthr = 0.1  #not used any more
        self.dPHICLUSTERthr = 0.1  #not used any more
#        self.TrackExtrapolator=extrap

# ---------------------------------------------------------------
# TrigL2ElectronFex configurations
# ---------------------------------------------------------------

class L2ElectronFex_all(L2ElectronFexBase):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_all"):
        super(L2ElectronFex_all, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        
        # Select tracking algorithm
        self.TrackAlgoId = 9 # IDscan [1=SiTrack, 4=TRT, 0=use all, 5: SiTrack+IDScan]

        # Co-ordinates of calorimeter face for extrapolation 
        self.RCalBarrelFace = 1470.0*mm
        self.ZCalEndcapFace = 3800.0*mm

        
class L2ElectronFex_LowPt(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_1"):
        super(L2ElectronFex_1, self).__init__(name)
        self.AcceptAll = False

class L2ElectronFex_HighPt(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_1"):
        super(L2ElectronFex_1, self).__init__(name)
        self.AcceptAll = False
        self.TrackPt = 2.0 * GeV

class L2ElectronFex_all_L2SW(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_all_L2SW"):
        super(L2ElectronFex_all_L2SW, self).__init__(name)

#No Cut chains
class L2ElectronFex_all_NoCut(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_all_NoCut"):
        super(L2ElectronFex_all_NoCut, self).__init__(name)
        self.AcceptAll = True

class L2ElectronFex_1(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_1"):
        super(L2ElectronFex_1, self).__init__(name)
        self.AcceptAll = True

class L2ElectronFex_all_L2SW_NoCut(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_all_L2SW_NoCut"):
        super(L2ElectronFex_all_L2SW_NoCut, self).__init__(name)
        self.AcceptAll = True

class L2ElectronFex_all_Ringer_NoCut(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_all_Ringer_NoCut"):
        super(L2ElectronFex_all_Ringer_NoCut, self).__init__(name)
        self.CaloAlgoId = 2 #CaloRinger
        self.AcceptAll = True

#L2 IDScan chains
class L2ElectronFex_IdScan(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_IdScan"):
        super(L2ElectronFex_IdScan, self).__init__(name)
        self.TrackAlgoId = 2 # IDscan 

class L2ElectronFex_IdScan_NoCut(L2ElectronFex_IdScan):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_IdScan_NoCut"):
        super(L2ElectronFex_IdScan_NoCut, self).__init__(name)
        self.AcceptAll = True

#L2 SiTrack chains
class L2ElectronFex_SiTrack(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_SiTrack"):
        super(L2ElectronFex_SiTrack, self).__init__(name)
        self.TrackAlgoId = 1 # SiTrack

class L2ElectronFex_SiTrack_NoCut(L2ElectronFex_SiTrack):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_SiTrack_NoCut"):
        super(L2ElectronFex_SiTrack_NoCut, self).__init__(name)
        self.AcceptAll = True

#L2 TRTSegFinder chains
class L2ElectronFex_TRTSegFinder(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_TRTSegFinder"):
        super(L2ElectronFex_TRTSegFinder, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        
        # Select tracking algorithm
        self.TrackAlgoId = 3 #TRTSegFinder

        # Calo-Tracking matching cuts
        self.TrackPtTRT = 1.0 * GeV
        self.CaloTrackdETATRT = [999., 999., 999., 999., 999., 999., 999., 999., 999.]
        self.CaloTrackdPHITRT = [0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]
     

class L2ElectronFex_TRTSegFinder_NoCut(L2ElectronFex_TRTSegFinder):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_TRTSegFinder_NoCut"):
        super(L2ElectronFex_TRTSegFinder_NoCut, self).__init__(name)
        self.AcceptAll = True

#L2 Use all tracks, EF forward+back tracking for photon conversion studies
class L2ElectronFex_FwdBackTracking(L2ElectronFexBase):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_FwdBackTracking"):
        super(L2ElectronFex_FwdBackTracking, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        
        # Select tracking algorithm
        self.TrackAlgoId = 0 # Use all tracks

        # Calo-Tracking matching cuts
        self.TrackPtTRT = 1.0 * GeV
        self.CaloTrackdETATRT = [999., 999., 999., 999., 999., 999., 999., 999., 999.]
        self.CaloTrackdPHITRT = [0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]
           
# L2Star strategy A,B,C
class L2ElectronFex_L2StarA(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_L2StarA"):
        super(L2ElectronFex_L2StarA, self).__init__(name)
        self.TrackAlgoId = 5 # L2Star strategy A

class L2ElectronFex_L2StarB(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_L2StarB"):
        super(L2ElectronFex_L2StarB, self).__init__(name)
        self.TrackAlgoId = 6 # L2Star strategy B

class L2ElectronFex_L2StarC(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_L2StarC"):
        super(L2ElectronFex_L2StarC, self).__init__(name)
        self.TrackAlgoId = 7 # L2Star strategy C

class L2ElectronFex_FTK(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_FTK"):
        super(L2ElectronFex_FTK, self).__init__(name)
        self.TrackAlgoId = 8

class L2ElectronFex_IDComb(L2ElectronFex_FwdBackTracking):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_IDComb"):
        super(L2ElectronFex_IDComb, self).__init__(name)


class L2ElectronFex_FwdBackTracking_NoCut(L2ElectronFex_FwdBackTracking):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_FwdBackTracking_NoCut"):
        super(L2ElectronFex_FwdBackTracking_NoCut, self).__init__(name)
        self.AcceptAll = True
      
class L2ElectronFex_Ringer(L2ElectronFex_all):
    __slots__ = []
    def __init__(self,name="L2ElectronFex_Ringer"):
        super(L2ElectronFex_Ringer, self).__init__(name)
        self.CaloAlgoId = 2 # CaloRinger

