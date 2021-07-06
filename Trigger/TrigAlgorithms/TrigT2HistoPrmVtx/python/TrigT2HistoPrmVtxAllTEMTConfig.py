# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConf import TrigT2HistoPrmVtxAllTE


from AthenaCommon.SystemOfUnits import mm, GeV


class TrigT2HistoPrmVtxAllTEBase (TrigT2HistoPrmVtxAllTE):
    __slots__ = []
    def __init__(self, name):
        super( TrigT2HistoPrmVtxAllTEBase, self ).__init__( name )


class TrigT2HistoPrmVtxAllTE_EF (TrigT2HistoPrmVtxAllTEBase):
    __slots__ = []
    def __init__(self, name = "TrigT2HistoPrmVtxAllTE_EF"):
        super( TrigT2HistoPrmVtxAllTE_EF, self ).__init__( name )

        self.NumBins            = 20
        
        self.UseBeamSpot        = True
        self.UseEtaPhiTrackSel  = False
        
        self.HLTTrkSel_Chi2      = 0.0
        self.HLTTrkSel_BLayer    = 1
        self.HLTTrkSel_PixHits   = 2
        self.HLTTrkSel_SiHits    = 7
        self.HLTTrkSel_D0        = 1*mm
        self.HLTTrkSel_Pt        = 1*GeV



class EFHistoPrmVtxAllTE_Jet (TrigT2HistoPrmVtxAllTE_EF):
    __slots__ = []
    def __init__(self, name = "EFHistoPrmVtxAllTE_Jet"):
        super( EFHistoPrmVtxAllTE_Jet, self ).__init__( name )

        self.AlgoId = 3
