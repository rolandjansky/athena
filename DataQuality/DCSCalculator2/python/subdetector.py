# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from __future__ import with_statement, division, print_function

from DQUtils import process_iovs
from DQUtils.sugar import IOVSet, define_iov_type
from DQUtils.ext import tally
import six

import DCSCalculator2.config as config
from .variable import GoodIOV, DCSC_Variable_With_Mapping, DefectIOV
from .consts import (WHITE, GREY, RED, YELLOW, GREEN, 
                     EMPTY, OUT_OF_CONFIG, BAD, GOOD)

import logging
log = logging.getLogger("DCSCalculator2.subdetector")
logEnabledFor = log.isEnabledFor


@define_iov_type
def DCSOFL_IOV(channel, Code, deadFrac, Thrust, NConfig, NWorking):
    "DCS calculator result iov type"

class DCSC_Subdetector(object):
    """
    A defect calculator for one subdetector.
    """
    
    def __init__(self):

        # calculate the inverse mapping if appropriate
        if not hasattr(self, "input_to_output_map") and hasattr(self, "mapping"):
            # NOTE: this breaks silently if an input channel was accidentally
            # mapped to more than one output channel. Maybe I should add a check.
            inverse = dict((value, key) 
                           for key, values in six.iteritems(self.mapping) 
                           for value in values)
                           
            self.input_to_output_map = inverse
            
    def __repr__(self):
        return self.__class__.__name__
    
    def get_variable(self, name):
        """
        Get a DCS_Variable by name.
        """
        for variable in self.variables:
            if variable.folder_name == name:
                return variable
                
        raise RuntimeError("Folder '%s' not found" % name)
                
    def set_input_mapping(self, what, mapping):
        """
        Set mapping of input channels for a DCSC_Variable_With_Mapping.
        Some input folders have different channel numbering conventions.
        """
        variable = self.get_variable(what)
        # Mapping only makes sense for DCSC_Variable_With_Mapping
        if not isinstance(variable, DCSC_Variable_With_Mapping):
            raise RuntimeError("'%s' is not a DCSC_Variable_With_Mapping!")
            
        variable.input_channel_map = mapping
        
    def evaluate_inputs(self, lbtime):
        """
        Read the cool database and determine the state of the input channels
        by luminosity block
        """

        # inputs is literally the same as self.variables
        #    # Is this true?? I don't think it is
        # calculate_good_iovs will calculate the goodness of all input channels
        # and remap the channel ids if necessary.  
        # TODO: This could be rewritten to be more clear.
        inputs = [v.calculate_good_iovs(lbtime, self) for v in self.variables]

        # Why do we care what the hash value is for the variables??
        if log.isEnabledFor(logging.INFO):
            log.info("lbtime hash = % 09x, inputs hash = %09x",
                     hash(lbtime), hash(tuple(self.variables)))
        return inputs
    
    def merge_variable_states(self, states):
        """
        Merge input channel states across variables, taking the worst.
        
        For detector configuration variables, it is assumed that if no IoV 
        exists for that channel in this variable, then it is in a good state.
        """

        # Start off with a good result.
        result = True

        # This assumes that the states array is in sync with the variables array..
        # Maybe I could add an assert statement here.
        # WOA!  That's not cool!  TODO: MAKE THIS BETTER
        # I don't think it has broken anything before but depending on how the variables
        # are defined in a subdetector I think this could be very bad!!!

        # loop over states
        for state, variable in zip(states, self.variables):
            state = state.good if state else None
            if state == OUT_OF_CONFIG:
                assert variable.is_config_variable, "OOC without is_config_variable!"
                # If any state is out of config, we know the result.
                return state
                
            elif state is None or state < result:
                if state is WHITE and variable.is_config_variable:
                    # Empty config variables are equivalent to "GOOD", so these
                    # states should be skipped.
                    continue
                result = state
                
        # More simplistic way of doing the above, but can't handle config vars:
        # return min(None if not state else state.good for state in states)
    
        return result

        
    def merge_inputs(self, channel, *inputs):
        """
        Merge multiple variables together for one input channel.
        Each 'inputs' arg is an IOVSet that corresponds to this input channel.
        """
        # inputs must correspond to and be in sync with subdetector.variables...?

        result = IOVSet()
        # combine the IOVSets into smallest chunks using process_iovs
        for since, until, states in process_iovs(*inputs):
            # Get the worst state for the list of vectors
            state = self.merge_variable_states(states)
            result.add(since, until, channel, state)
            
        return result.solidify(GoodIOV)
    
    def merge_input_information(self, channel, *inputs):
        """
        Join up the information which was used to make a decision across 
        multiple variables.
        """
        result = IOVSet()
        for since, until, states in process_iovs(*inputs):
            info = tuple(state._orig_iov[3:] for state in states)
            result.add(since, until, channel, info)
            
        return result.solidify(GoodIOV) 
    
    # inputs is a USELESS name for such an object
    def merge_input_variables(self, inputs):
        """
        Merge multiple variables together for many channels.
        Takes a list of IOVSets, one for each DCSC_Variable.
        """

        result = []
        info_states = IOVSet(iov_type=GoodIOV)
        
        # Reassign inputs to be a list of dictionaries with
        # Key=channel_id and Val=IOVSet
        inputs = [iovs.by_channel for iovs in inputs]

        # set of channel ids
        all_channels = sorted(set(y for x in inputs for y in x.keys()))
        
        tally_system_states = config.opts.tally_system_states
        
        for channel in all_channels:

            # Handle one channel at a time for variable merging
            c_inputs = [x[channel] for x in inputs]
            
            # Merge "good" state across multiple variables
            result.append(self.merge_inputs(channel, *c_inputs))
            
            if tally_system_states:
                # Merge Input information across multiple variables
                info_state = self.merge_input_information(channel, *c_inputs)
                info_states.extend(info_state)

        
        if tally_system_states:
            # Print a tally of the states for the different systems
            from DQUtils.ext import tally
            def pretty(state):
                return "/".join(x[0] for x in state)
            
            chans, iovs = zip(*sorted(six.iteritems(info_states.by_channel)))
            for since, until, states in process_iovs(self.run_iovs, *iovs):
                if states[0]._is_empty:
                    # Not inside a run
                    continue
                
                statetally = tally(pretty(x.good) for x in states[1:])
                
                print(since, until, statetally)
            
        return result
        
    def map_inputs_to_outputs(self, inputs):
        """
        Determine which input channels belong to which output channels.
        inputs is a list of IOVSets, exactly one per channel.
        """
        # Per output object, store a list of iov lists.
        result = {}
        
        # Keep a record of the channels we have seen, and what their index
        # will be in the states list. This is so that we can determine later
        # which channel a state belongs to. (For empty IoVs, for instance)
        self.channel_indices = {}
        seen_channels = set()
        
        empty_iovset_types = [iovs.empty_maker() for iovs in inputs]
        
        # input to output map is the inverse map with
        # key=input_channel and val=output_channel
        mapping = self.input_to_output_map

        # Loop over channels
        for iovs in inputs:
            if not iovs: continue # Can this happen?
            input_channel = iovs[0].channel
            if input_channel not in mapping: 
                # TODO: This could be an error.
                assert False, "channel not found in mapping: " + str(input_channel)
                continue
            seen_channels.add(input_channel)
            output_channel = mapping[input_channel]
            result.setdefault(output_channel, []).append(iovs)
            self.channel_indices.setdefault(output_channel, []).append(input_channel)
        
        missing_channels = self.input_channel_set - seen_channels
        
        for channel, make_iovset in zip(missing_channels, empty_iovset_types):
            # No IoVs for this channel. Append an empty IoV range.
            output_channel = mapping[channel]
            result.setdefault(output_channel, []).append(make_iovset())
            (self.channel_indices.setdefault(output_channel, [])
                                 .append(channel))
            
        return result
    
    @property
    def input_channel_set(self):
        """
        Return a set containing the all input channel IDs for this subdetector
        """
        return set(v for vals in self.mapping.values() for v in vals)
    
    def get_name_for_input_channel(self, input_channel):
        """
        If it is possible to give a logical name for an input channel, return
        it here. These numbers are used for debugging purposes.
        
        By default, do nothing. Over-ridden by subdetectors
        """
        return input_channel
    
    def get_ids_which_are(self, output_channel, states, what):
        indices = [i for i, x in enumerate(states) if x is what]
        chan_indices = self.channel_indices[output_channel]
        input_chan_name = self.get_name_for_input_channel
        return [input_chan_name(chan_indices[i]) for i in indices]
        
    def calculate_dead_fraction(self, since, until, output_channel, states, 
                                state_iovs):
        """
        Calculate the dead fraction and the resulting traffic light code.
        """

        #states = [s.good if s is not None else None for s in state_iovs]
        
        n_total    = len(states)
        n_working  = states.count(GOOD)
        n_bad      = states.count(BAD)
        n_ooc      = states.count(OUT_OF_CONFIG)
        n_unfilled = states.count(EMPTY)
        
        log.debug("%s -> %s tot:%4s working:%4s bad:%4s ooc:%4s empty:%4s",
                     since, until, n_total, n_working, n_bad, n_ooc, n_unfilled)
        
        # Reminder to self:
        # Need to take total from the right hand side here, ultimately.
        # Perhaps the correct method is to insert "empty IoVs" for missing
        # channels
        assert n_total == len(self.mapping[output_channel])
        
        if logEnabledFor(logging.DEBUG):
            if n_unfilled:
                args = output_channel, states, EMPTY
                unfilled_chans = sorted(self.get_ids_which_are(*args))
                log.debug("WARNING: the following channelids are unfilled: "
                          "%r", unfilled_chans)
                
            if n_ooc:
                ooc_ids = self.get_ids_which_are(output_channel, states, OUT_OF_CONFIG)
                log.debug("OOC ids: %s", sorted(ooc_ids))
            if n_bad:
                bad_ids = self.get_ids_which_are(output_channel, states, BAD)
                log.debug("BAD ids: %s", sorted(bad_ids))
        
        assert not n_total - n_working - n_bad - n_ooc - n_unfilled, (
            "Some states unaccounted for? This is a bug.")
        
        n_config = n_total - n_ooc
        dead_fraction = 1. - n_working / n_total

        code = GREEN
        if self.dead_fraction_caution is not None and (
           self.dead_fraction_caution < dead_fraction <= self.dead_fraction_bad):
            code = YELLOW
            
        elif dead_fraction > self.dead_fraction_bad:
            code = RED
        
        # If the number of unfilled channels is sufficient to send us over the 
        # caution threshold, then set the code to GREY to indicate a problem.
        unfilled_fraction = n_unfilled / n_total
        threshold = (self.dead_fraction_caution 
                     if self.dead_fraction_caution is not None else 
                     self.dead_fraction_bad)
        if unfilled_fraction > threshold:
            code = GREY
        
        if n_unfilled and config.opts.mark_unfilled_grey:
            code = GREY
        
        # what the heck is thrust?
        thrust = 0.
        return code, dead_fraction, thrust, n_config, n_working
    
    def debug_what_changed(self, runlb, prev_states, states):
        """Apparently not used"""
        changes = [(a, b) for a, b in zip(prev_states, states) if a != b]
        log.debug("Changes at %s: %i: %s", runlb, len(changes), tally(changes))
            
    def calculate_dead_fraction_all(self, output_channel, local_variables):
        
        log.debug("Calculating dead fractions for output: %i", output_channel)

        # local_variables is a list of IOVSets (one for each input channel), 
        # for this output channel.
        # Why would you call it local_variables?
        
        #prev_states = []
        
        dead_frac_iovs = IOVSet()
        calc_dead_frac = self.calculate_dead_fraction
        # loop over smallest IOV chunks for this output channel
        for since, until, states in process_iovs(self.run_iovs, *local_variables):
            run_iov = states[0]
            # state_iovs is now a list of iovs, one for each input channel mapped
            # to this output channel
            state_iovs = states[1:]
            
            states = [s.good for s in state_iovs]
                
            if run_iov._is_empty:
                # Ignore regions outside runs.
                continue
            
            iov_state = calc_dead_frac(since, until, output_channel, 
                                       states, state_iovs)
                                       
            dead_frac_iovs.add(since, until, output_channel, *iov_state)
            
        return dead_frac_iovs.solidify(DCSOFL_IOV)
    
    def dq_worst(self, states):
        """
        Make a DQ-colour decision based on `states`
        """
        states = set([s.Code for s in states])
        if   RED    in states: return RED
        elif YELLOW in states: return YELLOW
        elif WHITE  in states: return WHITE
        elif GREEN  in states: return GREEN
        return RED
    
    def merge_globals(self, output_channel, dead_frac_iovs, global_variables):
        """
        Merge together global states to decide a final code
        
        If the dead fraction is unavailable, writes -1.
        """
        result_iovs = IOVSet()
        if self.run_iovs is not None:
            # run_iovs are used to constrain to what runs the calculator will
            # write. If there is a hole in `run_iovs`, then no records are emitted.
            state_ranges = process_iovs(self.run_iovs, dead_frac_iovs, *global_variables)
        else:
            state_ranges = process_iovs(dead_frac_iovs, *global_variables)
            
        for since, until, states in state_ranges:
            if self.run_iovs:
                # No run_iovs were specified, so run for all available input data
                run_iov, dead_frac_iov = states[:2]
                
                if run_iov._is_empty:
                    # We're outside of a run, don't write an IoV.
                    continue
                    
                states = states[1:]
            else:
                dead_frac_iov = states[0]
            
            if not dead_frac_iov._is_empty:
                dead_fraction, thrust, n_config, n_working = dead_frac_iov[4:]
            else:
                dead_fraction, thrust, n_config, n_working = -1., 0., -1, -1
                states = states[1:]
            
            code = self.dq_worst(states)
            state = dead_fraction, thrust, n_config, n_working
            
            if code is WHITE:
                # Don't write empty regions
                continue
            
            result_iovs.add(since, until, output_channel, code, *state)
        
        return result_iovs.solidify(DCSOFL_IOV)            
    
    def calculate_result_for_output(self, output_channel,
                                    local_variables, global_variables):
        """
        Calculate the iov extents and dead fractions for one output channel
        
        * If there are 'non-global' variables, evaluate the dead fraction, which
          effectively becomes a new global variable.
        * If there are no global variables, return the above as a result
        * If there are global variables, merge them together.
        """
        
        dead_frac_iovs = IOVSet(iov_type=DCSOFL_IOV)
        
        if local_variables:
            dead_frac_iovs = self.calculate_dead_fraction_all(output_channel, 
                                                              local_variables)
        
        if not global_variables:
            # There are no globals, we're done.
            return dead_frac_iovs
        
        return self.merge_globals(output_channel, dead_frac_iovs, global_variables)
        
    def select_globals(self, output_channel, input_globals):
        """
        Returns a list where each element is a list of (single channel) iovs.
        
        The `input_globals` may contain a list of iovs which has multiple 
        channels. This function may be over-ridden by inheriting classes to 
        select channels for this output channel.
        """
        global_iov_sets = []
        for input_global in input_globals:
            for channel, iovs in sorted(six.iteritems(input_global.by_channel)):
                global_iov_sets.append(iovs)
            
        return global_iov_sets
        
    def calculate_result(self, inputs_by_output, global_variables):
        """
        Terrible name for a method.
        Calculate the iov extents and dead fractions for all output channels.
        In other words, the IoVs to be written to DCSOFL for this subdetector.
        """
        result = IOVSet(iov_type=DCSOFL_IOV)
        
        # loop over output channel dictionary
        for channel, input_iovs in sorted(six.iteritems(inputs_by_output)):
            these_globals = self.select_globals(channel, global_variables)
            args = channel, input_iovs, these_globals
            result.extend(self.calculate_result_for_output(*args))
            
        return result
        
    def run(self, lbtime, run_iovs=None):
        """
        Run the DCSC for this subdetector.
        
        * Evaluate inputs
        * Merge input variables together
        * Calculate resulting IoVs to be written
        """
        
        self.run_iovs = run_iovs
        
        self.start()
        
        # evaluate_inputs will calculate the goodness of all channels
        # for each variable, remapping channel ids if necessary.
        # this 'variables' object is the same as self.variables
        #   -> hold this thought, I'm not sure this is correct
        variables = self.evaluate_inputs(lbtime)

        # separate variables into global and local.
        # They are lists of IOVSets
        # Each element of the list corresponds to a DCSC_Variable
        local_variables  = [v.iovs for v in variables if not v.is_global]
        global_variables = [v.iovs for v in variables if v.is_global]

        # Merge IOVSets into a list with one IOVSet per channel
        input_channel_states = self.merge_input_variables(local_variables)
        
        # We only have merged input_channel_states if there are local variables
        if input_channel_states:
            inputs_by_output = self.map_inputs_to_outputs(input_channel_states)
        
        else:
            # If there are no locals, we need an empty locals list per output
            inputs_by_output = dict((cid, []) for cid in self.mapping.keys())

        # Calculate the final output IOVs
        result_iovs = self.calculate_result(inputs_by_output, global_variables)
        
        self.done()
        
        return result_iovs
    
    def start(self):
        "Empty function called at start"
        
    def done(self):
        """
        An empty function which can be overloaded to do any needed 
        post-processing
        """

class DCSC_DefectTranslate_Subdetector(DCSC_Subdetector):
    """
    A defect calculator for one subsystem that still works in terms of color
    flags. The colors need to be translated into defects by building translators
    with the color_to_defect_translator static method.
    """

    def __init__(self, keep_dcsofl=False):
        super(DCSC_DefectTranslate_Subdetector, self).__init__()
        self.translators = []
        self.keep_dcsofl = keep_dcsofl
    
    def run(self, lbtime, run_iovs=None):
        """
        Run the DCSC for this subdetector.
        
        * Evaluate inputs
        * Merge input variables together
        * Calculate resulting IoVs to be written
        """
        flag_iovs = super(DCSC_DefectTranslate_Subdetector, self).run(lbtime, run_iovs)
        translated = sum((func(flag_iovs) for func in self.translators), [])
        if self.keep_dcsofl:
            translated += flag_iovs
        return translated

    @staticmethod
    def color_to_defect_translator(inflag, outdefect, badcolors=[RED]):
        def translator_core(flag_iovs):
            rv = [DefectIOV(since=iov.since, until=iov.until,
                            channel=outdefect, present=True,
                            comment='Bad Fraction: %.3f' % iov.deadFrac)
                  for iov in flag_iovs if iov.channel == inflag
                  and iov.Code in badcolors]
            
            return rv
        return translator_core

class DCSC_Subdetector_DefectsOnly(DCSC_Subdetector):
    """
    This calculator can be used if the calculator is only using defects
    """

    def run(self, lbtime, run_iovs):
        self.evaluate_inputs(lbtime)
        result = IOVSet()
        for variable in self.variables:
            result.extend(variable.iovs)
        return result
