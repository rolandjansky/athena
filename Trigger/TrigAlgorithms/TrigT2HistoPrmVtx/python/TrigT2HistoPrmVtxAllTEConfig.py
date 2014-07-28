# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConf import TrigT2HistoPrmVtxAllTE


from AthenaCommon.SystemOfUnits import mm, GeV


class TrigT2HistoPrmVtxAllTEBase (TrigT2HistoPrmVtxAllTE):
    __slots__ = []
    def __init__(self, name):
        super( TrigT2HistoPrmVtxAllTEBase, self ).__init__( name )



class TrigT2HistoPrmVtxAllTE_L2 (TrigT2HistoPrmVtxAllTEBase):
    __slots__ = []
    def __init__(self, name = "TrigT2HistoPrmVtxAllTE_L2"):
        super( TrigT2HistoPrmVtxAllTE_L2, self ).__init__( name )

        from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxAllTEMonitoring import TrigT2HistoPrmVtxAllTEValidationMonitoring_L2, TrigT2HistoPrmVtxAllTEOnlineMonitoring_L2
        validation = TrigT2HistoPrmVtxAllTEValidationMonitoring_L2()        
        online     = TrigT2HistoPrmVtxAllTEOnlineMonitoring_L2()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigT2HistoPrmVtxAllTE")
        time.TimerHistLimits = [0,2]

        self.AthenaMonTools = [ time, validation, online ]
        
        self.Instance           = "_L2"

        self.NumBins            = 20
        
        self.UseBeamSpot        = True
        self.UseEtaPhiTrackSel  = False

        self.L2TrkSel_Chi2      = 0.001
        self.L2TrkSel_BLayer    = 1
        self.L2TrkSel_SiHits    = 4
        self.L2TrkSel_D0        = 1*mm
        self.L2TrkSel_Pt        = 1*GeV



class L2HistoPrmVtxAllTE_SiTrack (TrigT2HistoPrmVtxAllTE_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtxAllTE_SiTrack"):
        super( L2HistoPrmVtxAllTE_SiTrack, self ).__init__( name )

        self.AlgoId = 1



class L2HistoPrmVtxAllTE_IDScan (TrigT2HistoPrmVtxAllTE_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtxAllTE_IDScan"):
        super( L2HistoPrmVtxAllTE_IDScan, self ).__init__( name )

        self.AlgoId = 2



class L2HistoPrmVtxAllTE_JetA (TrigT2HistoPrmVtxAllTE_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtxAllTE_JetA"):
        super( L2HistoPrmVtxAllTE_JetA, self ).__init__( name )

        self.AlgoId = 5



class L2HistoPrmVtxAllTE_JetB (TrigT2HistoPrmVtxAllTE_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtxAllTE_JetB"):
        super( L2HistoPrmVtxAllTE_JetB, self ).__init__( name )

        self.AlgoId = 6



class L2HistoPrmVtxAllTE_JetF (TrigT2HistoPrmVtxAllTE_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtxAllTE_JetF"):
        super( L2HistoPrmVtxAllTE_JetF, self ).__init__( name )

        self.AlgoId = 8



class L2HistoPrmVtxAllTE_JetFR (TrigT2HistoPrmVtxAllTE_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtxAllTE_JetFR"):
        super( L2HistoPrmVtxAllTE_JetFR, self ).__init__( name )

        self.AlgoId = 13



class L2HistoPrmVtxAllTE_Tau (TrigT2HistoPrmVtxAllTE_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtxAllTE_Tau"):
        super( L2HistoPrmVtxAllTE_Tau, self ).__init__( name )

        self.AlgoId = 1



class TrigT2HistoPrmVtxAllTE_EF (TrigT2HistoPrmVtxAllTEBase):
    __slots__ = []
    def __init__(self, name = "TrigT2HistoPrmVtxAllTE_EF"):
        super( TrigT2HistoPrmVtxAllTE_EF, self ).__init__( name )

        from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxAllTEMonitoring import TrigT2HistoPrmVtxAllTEValidationMonitoring_EF, TrigT2HistoPrmVtxAllTEOnlineMonitoring_EF
        validation = TrigT2HistoPrmVtxAllTEValidationMonitoring_EF()        
        online     = TrigT2HistoPrmVtxAllTEOnlineMonitoring_EF()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigT2HistoPrmVtxAllTE")
        time.TimerHistLimits = [0,2]

        self.AthenaMonTools = [ time, validation, online ]
        
        self.Instance           = "_EF"

        self.NumBins            = 20
        
        self.UseBeamSpot        = True
        self.UseEtaPhiTrackSel  = False
        
        self.EFTrkSel_Chi2      = 0.0
        self.EFTrkSel_BLayer    = 1
        self.EFTrkSel_PixHits   = 2
        self.EFTrkSel_SiHits    = 7
        self.EFTrkSel_D0        = 1*mm
        self.EFTrkSel_Pt        = 1*GeV



class EFHistoPrmVtxAllTE_Jet (TrigT2HistoPrmVtxAllTE_EF):
    __slots__ = []
    def __init__(self, name = "EFHistoPrmVtxAllTE_Jet"):
        super( EFHistoPrmVtxAllTE_Jet, self ).__init__( name )

        self.AlgoId = 3



class EFHistoPrmVtxAllTE_Tau (TrigT2HistoPrmVtxAllTE_EF):
    __slots__ = []
    def __init__(self, name = "EFHistoPrmVtxAllTE_Tau"):
        super( EFHistoPrmVtxAllTE_Tau, self ).__init__( name )

        self.AlgoId = 3

