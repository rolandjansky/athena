# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from .lib import DCSC_DefectTranslate_Subdetector, DCSC_Variable, DCSC_Global_Variable

CSCEA, CSCEC = 334, 335

class CSC(DCSC_DefectTranslate_Subdetector):
    
    folder_base = "/CSC/DCS"
    
    mapping = {
        CSCEA: range(  1,  65),
        CSCEC: range( 65, 129),
    }

    variables = [
        DCSC_Variable       ("LAYERSTATE", lambda iov: iov.HVState == iov.LVState == 1),
        DCSC_Global_Variable("GASSTATE",   lambda iov: iov.State == True),
    ]
    
    # If you change this please consult with the Muon groups.
    # It was decided to make it the same across CSC, MDT, RPC and TGC.
    dead_fraction_caution = None
    dead_fraction_bad = 0.1

    def __init__(self, *args, **kwargs):
        #kwargs['keep_dcsofl'] = True
        super(CSC, self).__init__(*args, **kwargs)
        self.translators = [CSC.color_to_defect_translator(flag, defect)
                            for flag, defect in ((CSCEA, 'MS_CSC_EA_STANDBY_HV'),
                                                 (CSCEC, 'MS_CSC_EC_STANDBY_HV'),
                                                 )]
