# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ..lib import DCSC_DefectTranslate_Subdetector, DCSC_Global_Variable

GREY, RED, YELLOW = 0, 1, 2
EMBA, EMBC, EMECA, EMECC, HECA, HECC, FCALA, FCALC = 202, 203, 204, 205, 214, 215, 224, 225

class LAr(DCSC_DefectTranslate_Subdetector):
    """
    This subdetector only uses one global variable, FSM.
    
    Since it only has a global variable, it is not possible to calculate a dead
    fraction, so this is set to -1, and the dead fraction logic is not called.
    """
    
    folder_base = "/LAR/DCS"
    
    # This mapping dictionary has a different meaning for LAr with respect
    # to the other systems. These channels represent represent 'global'
    # states which should be merged together for a DQ region, rather than 
    # channels which should be counted in a dead fraction.
    mapping = {
        EMBA: [1],
        EMBC: [2],
        EMECA: [3],
        EMECC: [4],
        FCALA: [28, 30],
        FCALC: [29, 31],
        HECA: [13, 14, 30],
        HECC: [8, 15, 31],
    }
    
    def select_globals(self, output_channel, input_globals):
        """
        Returns a list where each element is a list of (single channel) iovs.
        
        The purpose of this function is to map (one or more) channels in the 
        /LAR/DCS/FSM folder on to output DQ regions. These channels are merged
        together downstream from this function.
        
        The `input_globals` may contain a list of iovs which has multiple 
        channels. This function may be over-ridden by inheriting classes to 
        select channels for this output channel.
        """
        global_iov_sets = []
        belongs_to_output = set(self.mapping[output_channel])
        
        for input_global in input_globals:
            for input_channel, iovs in sorted(input_global.by_channel.items()):
                if input_channel in belongs_to_output:
                    global_iov_sets.append(iovs)
            
        return global_iov_sets
        
    variables = [
        DCSC_Global_Variable("FSM", 
                    evaluator=lambda iov: (iov.STATE == "READY" and (iov.STATUS == "OK" or iov.STATUS == "WARNING")),
            caution_evaluator=lambda iov: (iov.STATE == "UNKNOWN" or iov.STATE == "DEAD" or iov.STATUS == "ERROR")
            ),
        ]

    def __init__(self, *args, **kwargs):
        super(LAr, self).__init__(*args, **kwargs)
        self.translators = ([LAr.color_to_defect_translator(flag, defect, [RED])
                            for flag, defect in ((EMBA, 'LAR_EMBA_DCS_ERROR'),
                                                 (EMBC, 'LAR_EMBC_DCS_ERROR'),
                                                 (EMECA, 'LAR_EMECA_DCS_ERROR'),
                                                 (EMECC, 'LAR_EMECC_DCS_ERROR'),
                                                 (HECA, 'LAR_HECA_DCS_ERROR'),
                                                 (HECC, 'LAR_HECC_DCS_ERROR'),
                                                 (FCALA, 'LAR_FCALA_DCS_ERROR'),
                                                 (FCALC, 'LAR_FCALC_DCS_ERROR'),
                                                 )]
                            + [LAr.color_to_defect_translator(flag, defect, [YELLOW, GREY])
                            for flag, defect in ((EMBA, 'LAR_EMBA_DCS_WARNING'),
                                                 (EMBC, 'LAR_EMBC_DCS_WARNING'),
                                                 (EMECA, 'LAR_EMECA_DCS_WARNING'),
                                                 (EMECC, 'LAR_EMECC_DCS_WARNING'),
                                                 (HECA, 'LAR_HECA_DCS_WARNING'),
                                                 (HECC, 'LAR_HECC_DCS_WARNING'),
                                                 (FCALA, 'LAR_FCALA_DCS_WARNING'),
                                                 (FCALC, 'LAR_FCALC_DCS_WARNING'),
                                                 )])
