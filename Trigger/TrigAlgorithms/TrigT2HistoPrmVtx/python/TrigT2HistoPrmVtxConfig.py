# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConf import TrigT2HistoPrmVtx


from AthenaCommon.SystemOfUnits import mm, GeV


class TrigT2HistoPrmVtxBase (TrigT2HistoPrmVtx):
    __slots__ = []
    def __init__(self, name):
        super( TrigT2HistoPrmVtxBase, self ).__init__( name )



class TrigT2HistoPrmVtx_L2 (TrigT2HistoPrmVtxBase):
    __slots__ = []
    def __init__(self, name = "TrigT2HistoPrmVtx_L2"):
        super( TrigT2HistoPrmVtx_L2, self ).__init__( name )

        from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxMonitoring import TrigT2HistoPrmVtxValidationMonitoring_L2, TrigT2HistoPrmVtxOnlineMonitoring_L2
        validation = TrigT2HistoPrmVtxValidationMonitoring_L2()        
        online     = TrigT2HistoPrmVtxOnlineMonitoring_L2()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigT2HistoPrmVtx")
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



class L2HistoPrmVtx_SiTrack (TrigT2HistoPrmVtx_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtx_SiTrack"):
        super( L2HistoPrmVtx_SiTrack, self ).__init__( name )
        
        self.AlgoId = 1



class L2HistoPrmVtx_IDScan (TrigT2HistoPrmVtx_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtx_IDScan"):
        super( L2HistoPrmVtx_IDScan, self ).__init__( name )
        
        self.AlgoId = 2



class L2HistoPrmVtx_JetA (TrigT2HistoPrmVtx_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtx_JetA"):
        super( L2HistoPrmVtx_JetA, self ).__init__( name )

        self.AlgoId = 5



class L2HistoPrmVtx_JetB (TrigT2HistoPrmVtx_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtx_JetB"):
        super( L2HistoPrmVtx_JetB, self ).__init__( name )

        self.AlgoId = 6



class L2HistoPrmVtx_JetF (TrigT2HistoPrmVtx_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtx_JetF"):
        super( L2HistoPrmVtx_JetF, self ).__init__( name )

        self.AlgoId = 8



class L2HistoPrmVtx_JetFR (TrigT2HistoPrmVtx_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtx_JetFR"):
        super( L2HistoPrmVtx_JetFR, self ).__init__( name )

        self.AlgoId = 13



class L2HistoPrmVtx_Tau (TrigT2HistoPrmVtx_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtx_Tau"):
        super( L2HistoPrmVtx_Tau, self ).__init__( name )

        self.AlgoId = 1



class L2HistoPrmVtx_TauA (TrigT2HistoPrmVtx_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtx_TauA"):
        super( L2HistoPrmVtx_TauA, self ).__init__( name )

        self.AlgoId = 5



class L2HistoPrmVtx_TauIsoB (TrigT2HistoPrmVtx_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtx_TauIsoB"):
        super( L2HistoPrmVtx_TauIsoB, self ).__init__( name )

        self.AlgoId = 6



class L2HistoPrmVtx_TauIsoF (TrigT2HistoPrmVtx_L2):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtx_TauIsoF"):
        super( L2HistoPrmVtx_TauIsoF, self ).__init__( name )

        self.AlgoId = 8



class TrigT2HistoPrmVtx_EF (TrigT2HistoPrmVtxBase):
    __slots__ = []
    def __init__(self, name = "TrigT2HistoPrmVtx_EF"):
        super( TrigT2HistoPrmVtx_EF, self ).__init__( name )

        from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxMonitoring import TrigT2HistoPrmVtxValidationMonitoring_EF, TrigT2HistoPrmVtxOnlineMonitoring_EF
        validation = TrigT2HistoPrmVtxValidationMonitoring_EF()        
        online     = TrigT2HistoPrmVtxOnlineMonitoring_EF()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigT2HistoPrmVtx")
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



class EFHistoPrmVtx_Jet (TrigT2HistoPrmVtx_EF):
    __slots__ = []
    def __init__(self, name = "EFHistoPrmVtx_Jet"):
        super( EFHistoPrmVtx_Jet, self ).__init__( name )
        
        self.AlgoId = 3



class EFHistoPrmVtx_Tau (TrigT2HistoPrmVtx_EF):
    __slots__ = []
    def __init__(self, name = "EFHistoPrmVtx_Tau"):
        super( EFHistoPrmVtx_Tau, self ).__init__( name )

        self.AlgoId = 3




