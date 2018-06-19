# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from .lib import DCSC_DefectTranslate_Subdetector, DCSC_Variable

RPCBA, RPCBC = 312, 313

class RPC(DCSC_DefectTranslate_Subdetector):
    
    folder_base = "/RPC/DCS"
    
    mapping = {
        RPCBA: range(203, 405),
        RPCBC: range(  1, 203),
    }

    variables = [
        DCSC_Variable("DQTOWERS_3", lambda iov: iov.TowerDQFlag == 2),
    ]

    # If you change this please consult with the Muon groups.
    # It was decided to make it the same across CSC, MDT, RPC and TGC.
    dead_fraction_caution = None
    dead_fraction_bad = 0.1

    def __init__(self, *args, **kwargs):
        #kwargs['keep_dcsofl'] = True
        super(RPC, self).__init__(*args, **kwargs)
        self.translators = [RPC.color_to_defect_translator(flag, defect)
                            for flag, defect in ((RPCBA, 'MS_RPC_BA_STANDBY_HV'),
                                                 (RPCBC, 'MS_RPC_BC_STANDBY_HV'),
                                                 )]
