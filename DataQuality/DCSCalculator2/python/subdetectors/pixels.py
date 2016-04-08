# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from .lib import DCSC_Subdetector, DCSC_Variable, GoodIOV, OUT_OF_CONFIG

from DQUtils.events import process_iovs_mc
from DQUtils.sugar import IOVSet

import logging
logger = logging.getLogger("DCSCalculator2.subdetectors.pixels")

WHITE, BLACK, GREY, RED, YELLOW, GREEN = None, -1, 0, 1, 2, 3

class DCSC_Variable_Pixels_Config(DCSC_Variable):
    
    is_config_variable = True
    timewise_folder = False
    input_db = "COOLONL_TDAQ/COMP200"
    fetch_args = dict(what=[])
    
    def make_good_iovs(self, iovs):
        """
        The absence of an IoV means that channel is out of config.
        """
        
        seen_channels = set()
        result = IOVSet()
        for since, until, channel, (state,) in process_iovs_mc(iovs):
            seen_channels.add(channel)
            if state._is_empty:
                # There is an IoV hole for this channel.
                result.add(since, until, channel, OUT_OF_CONFIG)
        
        # Need to deal with channels which were not seen at all for the query 
        # range, since they will not appear in the above loop
        all_channels = self.subdetector.input_channel_set
        
        for missing_channel in (all_channels - seen_channels):
            result.add(0, 2**63-1, missing_channel, OUT_OF_CONFIG)
                
        return result.solidify(GoodIOV)

class Pixels(DCSC_Subdetector):
    
    folder_base = "/PIXEL/DCS"
    
    cid_barrel,  cid_blayer  = 101, 102
    cid_endcapa, cid_endcapb = 104, 105
    
    mapping = {
        101: range( 430, 1600),
        102: range( 144,  430),
        104: range(1600, 1744),
        105: range(   0,  144),
    }

    variables = [
        # Pixel allows FSM status to be OK or WARNING
        #DCSC_Variable("FSMSTATUS", lambda iov: iov.FSM_status == "OK"),
        DCSC_Variable("FSMSTATUS", lambda iov: iov.FSM_status in ("OK", "WARNING")),
        DCSC_Variable("FSMSTATE",  lambda iov: iov.FSM_state  == "READY"),
        #DCSC_Variable_Pixels_Config("/TDAQ/EnabledResources/ATLAS/PIXEL/Modules", None),
    ]

    # Note barrel and blayer use different deadfraction_caution. Implemented in 
    # calculate_iov below.
    dead_fraction_caution = 0.2
    dead_fraction_caution_barrel = 0.05
    dead_fraction_bad = 0.9
    
    assert dead_fraction_caution_barrel <= dead_fraction_caution, (
        "logic needs changing in calculate_iov before removing this assert")
    
    def start(self):
        self.bad_modules = IOVSet()
        
    def tally_additional_info(self, since, until, output_channel, 
                              states, state_iovs):
        """
        Figure out which modules are OOC when
        """
        ooc_modules = self.get_ids_which_are(output_channel, states, OUT_OF_CONFIG)
        ooc_modules = tuple(sorted(ooc_modules))
        self.bad_modules.add(since, until, output_channel, ooc_modules)
    
    def done(self):
        if logger.isEnabledFor(logging.DEBUG):
            print "The following ranges indicate bad modules:"
            from pprint import pprint
            pprint(list(self.bad_modules))
    
    def calculate_dead_fraction(self, since, until, output_channel, states, 
                                state_iovs):
        """
        Compute the dead fraction differently for the barrel and blayer.
        
        This function is written with the assumption that 
        `dead_fraction_caution_barrel` is smaller than `dead_fraction_caution`
        because the new logic is only run if the code is better than Yellow.        
        """
        cdf = super(Pixels, self).calculate_dead_fraction
        result = cdf(since, until, output_channel, states, state_iovs)
        
        self.tally_additional_info(since, until, output_channel, states, state_iovs)
        
        code, dead_fraction, thrust, n_config, n_working = result
                                                   
        if (output_channel not in [self.cid_barrel, self.cid_blayer] or 
            code <= YELLOW):
            # Result is already correct
            return result
        
        # Need to check whether code needs modifying.
        if self.dead_fraction_caution_barrel < dead_fraction < self.dead_fraction_bad:
            code = YELLOW
        
        return code, dead_fraction, thrust, n_config, n_working 
        
    def initialize_name_mapping(self):
        from DQUtils.db import Databases, get_channel_ids_names
        
        f = Databases.get_folder("/TDAQ/EnabledResources/ATLAS/PIXEL/Modules", "COOLONL_TDAQ")
        cids, cnames, cmap = get_channel_ids_names(f)
        self.name_mapping = dict(zip(cids, cnames))
        self.name_mapping_initialized = True
        
    def get_name_for_input_channel(self, input_channel):
        """
        Transform an input channelid into a name
        """
        if not getattr(self, "name_mapping_initialized", False):
            self.initialize_name_mapping()
        
        if input_channel in self.name_mapping:
            return (input_channel, self.name_mapping[input_channel])
        
        return input_channel
        
