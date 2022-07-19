# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from ..lib import DCSC_DefectTranslate_Subdetector, DCSC_Variable

TGCEA, TGCEC = 324, 325

def TGC_HV_state(iov):
    return iov.fsm_currentState == "ON"

class TGC(DCSC_DefectTranslate_Subdetector):

    folder_base = "/TGC/DCS"

    #
    # how to derive mapping:
    # ----------------------
    # from DQUtils                 import Databases
    # from DQUtils.channel_mapping import get_channel_ids_names
    # folder, database = "/TGC/DCS/PSHVCHSTATE", "COOLOFL_DCS/CONDBR2"
    # ids, names, _ = get_channel_ids_names(Databases.get_folder(folder, database))
    # for id, name in zip(ids, names):
    #     print id, name
    #
    mapping = {
        TGCEA: list(range(3733, 5504)) + list(range(5552, 5593)) + list(range(7459, 7465)),
        TGCEC: list(range(5593, 7362)) + list(range(7411, 7459))
    }

    variables = [
        DCSC_Variable("PSHVCHSTATE", TGC_HV_state),
        # DCSC_Variable("PSHVVMON", TGC_HV_value),
    ]

    # If you change this please consult with the Muon groups.
    # It was decided to make it the same across CSC, MDT, RPC and TGC.
    dead_fraction_caution = None
    dead_fraction_bad = 0.1

    def __init__(self, *args, **kwargs):

        #kwargs['keep_dcsofl'] = True

        super(TGC, self).__init__(*args, **kwargs)

        self.translators = [TGC.color_to_defect_translator(flag, defect)
                            for flag, defect in ((TGCEA, 'MS_TGC_EA_STANDBY_HV'),
                                                 (TGCEC, 'MS_TGC_EC_STANDBY_HV'),
                                                 )]
