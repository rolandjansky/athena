# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import with_statement, division

import logging; log = logging.getLogger("DCSCalculator2.variable")

from DQUtils import fetch_iovs
from DQUtils.general import timer
from DQUtils.sugar import define_iov_type, RunLumi, IOVSet, RANGEIOV_VAL
from DQUtils.events import quantize_iovs_slow_mc

import DCSCalculator2.config as config
from DCSCalculator2.libcore import map_channels
from DCSCalculator2.consts import ( RED, YELLOW, GREEN )


@define_iov_type
def GoodIOV(channel, good):
    "Stores the state of a single input channel, whether it is working or not"

@define_iov_type
def CodeIOV(channel, Code):
    "Similar to GoodIOV but stores a DQ code instead of good/bad state"

@define_iov_type
def DefectIOV(channel, present, comment):
    "Stores a defect IOV"

class DCSC_Variable(object):
    """
    Class which encapsulates logic behind an input variable.
    
    This class is responsible for:
    
    * Reading data from COOL / CoraCOOL
    * Evaluating the 'good' state of a variable once read from the database
    * Quantizing the state from time-based intervals of validity to lumiblock
    
    It is subclassed for configuration variables and "global" variables.
    """
 
    # defaults for a variable
    is_global = False
    is_config_variable = False
    timewise_folder = True

    def __init__(self, folder_name, evaluator, **kwargs):
        if not hasattr(self, 'input_db'):
            self.input_db = 'COOLOFL_DCS/CONDBR2'
        self.folder_name = folder_name
        self.evaluator = evaluator
        if not hasattr(self, "fetch_args"):
            self.fetch_args = {}
        self.__dict__.update(kwargs)
        self.input_hashes = []
    
    def __hash__(self):
        """
        Useful for verifying if input variables have changed.
        """
        # Consider the fact that hash((hash(x), hash(y))) == hash((x, y)) where
        # x and y are tuples.
        return hash(tuple(self.input_hashes))
    
    def __repr__(self):
        return "<DCSCVariable %s>" % self.folder_name
    
    def read(self, query_range, folder_base, folder_name):
        """
        Read the relevant data from COOL for this variable
        """
        if folder_name.startswith("/"):
            folder_path = folder_name
        else:
            # For relative folders prepend the folder_base
            folder_path = "/".join((folder_base, folder_name))
        
        log.info("Querying COOL folder %s", folder_path)
        
        if config.opts.check_input_time:
            self.fetch_args["with_time"] = True
        
        # Massage DB access
        if '/' in self.input_db:
            newdbstring = self.input_db.rsplit('/', 1)[0]
        else:
            newdbstring = self.input_db
        if config.opts.input_database.startswith('sqlite'):
            self.fetch_args['database'] = config.opts.input_database
        else:
            self.fetch_args['database'] = ('%s/%s' % (newdbstring, config.opts.input_database))
        if self.fetch_args:
            log.debug("Fetching with args: %r", self.fetch_args)
            
        iovs = fetch_iovs(folder_path, *query_range, **self.fetch_args)

        # Prints even when not doing debug.
        # TODO: fix this. Might be broken in DQUtils.logger
        #if log.isEnabledFor(logging.DEBUG):
        #    log.debug("Dumping input IOVs:")
        #    for iov in iovs:
        #        print iov
        
        if config.opts.check_input_time:
            self.print_time_info(iovs)
            
        if log.isEnabledFor(logging.INFO):
            input_hash = hash(iovs)
            self.input_hashes.append(input_hash)
            log.info("  -> Input hash: % 09x (len=%i)", input_hash, len(iovs))
            
        return iovs
    
    def print_time_info(self, iovs):
        """
        Logs the first and last insertion times of the IoVs, and their ranges.
        """
        first_value, last_value = iovs.range_iov
        
        log.info("Times for %s:", self)
        log.info("  IoV      : (first)%26s (last)%26s", 
                    first_value.date, last_value.date)
        
        if not hasattr(iovs.first, "insertion_time"):
            log.info("Insertion time not available")
        else:
            insertion_times = [iov.insertion_time for iov in iovs]
            log.info("  Insertion: (first)%26s (last)%26s", 
                        min(insertion_times), max(insertion_times))
    
    def map_input_channels(self, iovs):
        """
        By default, do nothing. Overloaded by DCSC_Variable_With_Mapping.
        """
        return iovs
    
    def quantize(self, lbtime, iovs):
        """
        Quantize "good state" timewise-iovs to lumiblocks.
        OUT_OF_CONFIG gets priority over BAD if BAD and OUT_OF_CONFIG overlap
        the same lumiblock.
        """
        IOVSet = iovs.empty
        iovs = [iovs_ for c, iovs_ in sorted(iovs.by_channel.items())]
        
        def quantizer (iovs):
            return min(i.good for i in iovs) if iovs else None
        
        result = quantize_iovs_slow_mc(lbtime, iovs, quantizer)
        return IOVSet(GoodIOV(*iov) 
                      for iovs in result 
                      for iov in iovs if iov[0].run == iov[1].run)
    
    def make_good_iov(self, iov):
        """
        Determine if one input iov is good.
        """
        giov = GoodIOV(iov.since, iov.until, iov.channel, self.evaluator(iov))
        giov._orig_iov = iov
        return giov
    
    def make_good_iovs(self, iovs):
        """
        Determine whether each iov signifies a good or bad state.
        """
        make_good_iov = self.make_good_iov
        return IOVSet(make_good_iov(iov) for iov in iovs)
    
    def calculate_good_iovs(self, lbtime, subdetector):
        """
        Calculate LB-wise "good" states
        """

        self.subdetector = subdetector
        
        if self.timewise_folder:
            query_range = RANGEIOV_VAL(lbtime.first.since, lbtime.last.until)
        else:
            a, b = lbtime.first, lbtime.last
            query_range = RANGEIOV_VAL(RunLumi(a.Run, a.LumiBlock), 
                                       RunLumi(b.Run, b.LumiBlock))
        
        # Read the database
        iovs = self.read(query_range, subdetector.folder_base, self.folder_name)
        #iovs.pprint()

        # Decide the states of the input iovs
        iovs = self.make_good_iovs(iovs)
        #iovs.pprint()
        
        # Apply a mapping for input channels if necessary
        # This only does something for variables that require additional mapping
        # i.e., if the channel numbers for different DCS variables don't match up
        iovs = self.map_input_channels(iovs)

        if self.timewise_folder and not config.opts.timewise:
            # we might already know the defect mapping
            with timer("Quantize %s (%i iovs over %i lbs)" % 
                       (self.folder_name, len(iovs), len(lbtime))):
                # Quantize to luminosity block
                iovs = self.quantize(lbtime, iovs)
        #iovs.pprint()
            
        # Debug printout of problematic channels
        # DQUtils messes with the logging and isEnabledFor doesn't work
        #if log.isEnabledFor(logging.DEBUG):
        #log.verbose("Bad input channels for %s:", self.folder_name)
        #log.verbose("= [%r]", ", ".join(str(i.channel) for i in iovs if not i.good))

        self.iovs = iovs
        
        return self

class DCSC_Variable_With_Mapping(DCSC_Variable):
    """
    A variable which needs channel ids to be remapped before further use
    """
    def map_input_channels(self, iovs):
        return map_channels(iovs, self.input_channel_map, self.folder_name)

class DCSC_Global_Variable(DCSC_Variable):
    """
    A global variable.
    
    This class over-rides the behaviour for evaluating the "goodness" of an 
    input channel. It allows for an intermediate state (caution) between good
    and bad.
    """
    is_global = True
    
    def __init__(self, folder_name, evaluator, caution_evaluator=None, **kwargs):
        super(DCSC_Global_Variable, self).__init__(folder_name, evaluator, **kwargs)
        
        self.caution_evaluator = caution_evaluator
        
    def make_good_iov(self, iov):
        """
        Determine DQ colour for this global variable iov.
        """
        
        if self.evaluator(iov):
            state = GREEN
            
        elif self.caution_evaluator and self.caution_evaluator(iov):
            state = YELLOW
        
        else:
            state = RED
            
        return CodeIOV(iov.since, iov.until, iov.channel, state)

    def quantize(self, lbtime, iovs):
        """
        Needs a different quantizer. (The default DQ quantizer will do)
        """
        iovs = [iovs_ for c, iovs_ in sorted(iovs.by_channel.items())]
        # Custom quantizer not needed        
        result = quantize_iovs_slow_mc(lbtime, iovs)
        return IOVSet(CodeIOV(*iov) 
                      for iovs in result 
                      for iov in iovs 
                      if iov[0].run == iov[1].run)

class DCSC_Defect_Global_Variable(DCSC_Variable):
    """
    Global variable which emits defects
    """
    is_global = True
    
    def __init__(self, folder_name, evaluator, **kwargs):
        super(DCSC_Defect_Global_Variable, self).__init__(folder_name, evaluator, **kwargs)

    @staticmethod
    def quantizing_function(current_events):
        if len(current_events) == 0:
            return None
        else:
            return True
            # Ideally, here we would somehow choose a comment to use, however
            # there is no general way to do this at the moment which makes sense.
            #return (True, list(current_events)[0].comment)
        
    def quantize(self, lbtime, iovs):
        iovs = [iovs_ for c, iovs_ in sorted(iovs.by_channel.items())]
        result = quantize_iovs_slow_mc(lbtime, iovs,
                                       DCSC_Defect_Global_Variable.quantizing_function)
        return IOVSet(DefectIOV(*iov, comment='Automatically set') 
                      for iovi in result 
                      for iov in iovi 
                      if iov[0].run == iov[1].run)
