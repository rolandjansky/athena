# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from .lib import DCSC_Subdetector, DCSC_Global_Variable

class LAr(DCSC_Subdetector):
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
        202: [1],
        203: [2],
        204: [3],
        205: [4],
        224: [28, 30],
        225: [29, 31],
        214: [13, 14, 30],
        215: [8, 15, 31],
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
            for input_channel, iovs in sorted(input_global.by_channel.iteritems()):
                if input_channel in belongs_to_output:
                    global_iov_sets.append(iovs)
            
        return global_iov_sets
        
    variables = [
        DCSC_Global_Variable("FSM", 
                    evaluator=lambda iov: (iov.STATE == "READY" and (iov.STATUS == "OK" or iov.STATUS == "WARNING")),
            caution_evaluator=lambda iov: (iov.STATE == "UNKNOWN" or iov.STATE == "DEAD" or iov.STATUS == "ERROR")
            ),
        ]

