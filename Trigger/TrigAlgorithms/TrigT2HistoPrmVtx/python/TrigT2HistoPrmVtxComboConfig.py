# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigT2HistoPrmVtx.TrigT2HistoPrmVtxConf import TrigT2HistoPrmVtxCombo

class TrigT2HistoPrmVtxComboBase (TrigT2HistoPrmVtxCombo):
    __slots__ = []
    def __init__(self, name):
        super( TrigT2HistoPrmVtxComboBase, self ).__init__( name )

        

class L2HistoPrmVtxCombo_Jet (TrigT2HistoPrmVtxComboBase):
    __slots__ = []
    def __init__(self, name = "L2HistoPrmVtxCombo_Jet"):
        super( L2HistoPrmVtxCombo_Jet, self ).__init__( name )

        

class EFHistoPrmVtxCombo_Jet (TrigT2HistoPrmVtxComboBase):
    __slots__ = []
    def __init__(self, name = "EFHistoPrmVtxCombo_Jet"):
        super( EFHistoPrmVtxCombo_Jet, self ).__init__( name )

        



