# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from cPickle import loads
from pkg_resources import resource_string

from .lib import (DCSC_DefectTranslate_Subdetector, DCSC_Variable, GoodIOV, OUT_OF_CONFIG,
                  DCSC_Variable_With_Mapping)

from DQUtils.sugar import IOVSet

class DCSC_Variable_SCT_Config(DCSC_Variable_With_Mapping):
    """
    Specialisation for the /SCT/DAQ/Configuration/Module folder.
    
    CoraCOOL stores multiple elements per iov. This folder only ever contains 
    zero or one elements.
    """
    
    input_db = "COOLONL_SCT/CONDBR2"
    is_config_variable = True
    
    # This is an instruction to fetch_iovs to get the "group" and "id" 
    # variables from CoraCOOL
    fetch_args = dict(what=["group", "id"])
    
    def make_good_iovs(self, iovs):
        """
        Beware, deceptive function name. Here we are really 
        making OUT_OF_CONFIG iovs, rather than 'good' ones.
        
        The resulting IoVs are in terms of detector serial number. These get
        mapped onto the HV channel IDs with the `sct_sn_to_cool_ids` mapping.
        """
        iovs = [iov for iov in iovs if iov.elements]
        #assert all(len(iov.elements) == 1 for iov in iovs)
        iovs = [GoodIOV(iov.since, iov.until, iov.elements[0].id, OUT_OF_CONFIG) 
                for iov in iovs if iov.elements[0].group == -1]
        
        return IOVSet(iovs)

def make_sct_mapping():
    """
    Generate a mapping for the channelids in /SCT/DCS/HV to DCSOFL output 
    channel ids. The highest two bits can be used to determine the system.
    (see `subdets`)
    """
    from DQUtils.db import Databases
    from DQUtils.channel_mapping import get_channel_ids_names
    f = Databases.get_folder("/SCT/DCS/HV", "COOLOFL_DCS/CONDBR2")
    cids, cnames, cmap = get_channel_ids_names(f)
    
    mapping = {}
    
    subdets = {0: 114, 1: 111, 2: 115}
    
    for cid in cids:
        # The highest-two bits of cid specify which DQ region we're in
        output_subdet = subdets[(cid >> 25) & 0x3]
        mapping.setdefault(output_subdet, []).append(cid)
    
    return mapping

hi_nibble = lambda x: (x & 0xF0) >> 4
lo_nibble = lambda x: (x & 0x0F)

# Documented at: (Visited 20/05/2010)
# https://twiki.cern.ch/twiki/bin/view/Atlas/SctDCSSoftware#FSM_Channel_Status_Byte
C_GOOD, C_MANUAL = 0x1, 0x3
good_states = [C_GOOD, C_MANUAL]

def check_good_chanstat(iov):
    """
    Determine whether CHANSTAT iov is good or bad.
    """ 
    return (hi_nibble(iov.STATE) in good_states and
            lo_nibble(iov.STATE) in good_states)
             
class SCT(DCSC_DefectTranslate_Subdetector):

    folder_base = "/SCT/DCS"
    
    def __init__(self, *args, **kwargs):
        """
        Initialise two mappings: Input COOL ids to output ids, and serial
        numbers for the configuration need mapping onto input ids.
        """
        self.mapping = make_sct_mapping()
        #if 'keep_dcsofl' not in kwargs:
        #    kwargs['keep_dcsofl'] = True
        super(SCT, self).__init__(*args, **kwargs)

        sct_sn_to_cool_ids = resource_string("DCSCalculator2.subdetectors.data", 
                                             "sct_module_ids.dat")
        sct_sn_to_cool_ids = sct_sn_to_cool_ids.strip().split("\n")
        sn_mapping = dict(map(int, l.split()) for l in sct_sn_to_cool_ids if l)
        self.set_input_mapping("/SCT/DAQ/Configuration/Module", sn_mapping)
        self.translators = [SCT.sct_color_to_defect_translator]

    @staticmethod
    def sct_color_to_defect_translator(flag_iovs):
        defectmap = { 111: 'B', 114: 'EA', 115: 'EC' }
        from DCSCalculator2.consts import GREEN
        from DCSCalculator2.variable import DefectIOV
        from DQUtils import process_iovs
        rv = []
        for since, until, states in process_iovs(*flag_iovs.by_channel.values()):
            if any(state.Code != GREEN for state in states):
                # This line of code is awful!
                badfrac = 'Bad fractions: ' + ', '.join('%s %.3f' % (defectmap[state.channel], state.deadFrac) for state in sorted(states, key=lambda x: x.channel))
                rv.append(DefectIOV(since=since, until=until,
                                    channel='SCT_GLOBAL_STANDBY', present=True,
                                    comment = badfrac))
        return rv
    
    dead_fraction_caution = 0.1
    dead_fraction_bad = 0.5
    
    variables = [
        DCSC_Variable("HV", lambda iov: 55 < iov.HVCHVOLT_RECV < 3000),
        DCSC_Variable("CHANSTAT", check_good_chanstat),
        
        # Evaluator not required because make_good_iovs overloaded
        DCSC_Variable_SCT_Config("/SCT/DAQ/Configuration/Module", None),
    ]
