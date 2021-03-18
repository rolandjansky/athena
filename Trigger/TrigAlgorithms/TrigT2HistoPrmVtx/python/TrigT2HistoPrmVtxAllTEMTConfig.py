# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConf import TrigT2HistoPrmVtxAllTEMT


from AthenaCommon.SystemOfUnits import mm, GeV


class TrigT2HistoPrmVtxAllTEMTBase (TrigT2HistoPrmVtxAllTEMT):
    __slots__ = []
    def __init__(self, name):
        super( TrigT2HistoPrmVtxAllTEMTBase, self ).__init__( name )


class TrigT2HistoPrmVtxAllTEMT_EF (TrigT2HistoPrmVtxAllTEMTBase):
    __slots__ = []
    def __init__(self, name = "TrigT2HistoPrmVtxAllTEMT_EF"):
        super( TrigT2HistoPrmVtxAllTEMT_EF, self ).__init__( name )

        self.NumBins            = 20
        
        self.UseBeamSpot        = True
        self.UseEtaPhiTrackSel  = False
        
        self.HLTTrkSel_Chi2      = 0.0
        self.HLTTrkSel_BLayer    = 1
        self.HLTTrkSel_PixHits   = 2
        self.HLTTrkSel_SiHits    = 7
        self.HLTTrkSel_D0        = 1*mm
        self.HLTTrkSel_Pt        = 1*GeV



class EFHistoPrmVtxAllTEMT_Jet (TrigT2HistoPrmVtxAllTEMT_EF):
    __slots__ = []
    def __init__(self, name = "EFHistoPrmVtxAllTEMT_Jet"):
        super( EFHistoPrmVtxAllTEMT_Jet, self ).__init__( name )

        self.AlgoId = 3
