# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import logging; log = logging.getLogger("DCSCalculator2.lib")

from DQUtils.iov_arrangement import flatten_channels
from DQUtils.sugar import RunLumi, IOVSet

def dcsofl_cool_record():
    from PyCool import cool
    ST = cool.StorageType
    return [("Code",     ST.Int32),
             ("deadFrac", ST.Float),
             ("Thrust",   ST.Float),
             ("NConfig",  ST.Int32),
             ("NWorking", ST.Int32)]

def make_multi_mapping(iterable):
    """
    When more than one value can map to the same key, we need 
    {key, [value1, value2]}. 
    
    This function builds it out of [(key, value1), (key, value2)]
    
    Beware, values cannot be lists.
    """
    
    result = {}
    
    for key, value in iterable:
        if key in result and not isinstance(result[key], list):
            result[key] = [result[key], value]
        elif key in result:
            result[key].append(value)
        else:
            result[key] = value
        
    return result

def map_channels(iovs, mapping, folder):
    """
    Remap the input channel identifiers.  Returns a new IOVSet 
    with the channel number changed according to the provided mapping
    """

    # look for unmapped channels
    bad_channels = set()
    def has_channel(c):
        result = c in mapping
        if not result:
            bad_channels.add(c)
        return result
    
    IOVSet = iovs.empty
    iovs = IOVSet(iov._replace(channel=mapping[iov.channel]) 
                      for iov in iovs if has_channel(iov.channel))
    
    if bad_channels:
        log.debug("WARNING: %s has %i unmapped channels %r"
                  % (folder, len(bad_channels), repr(bad_channels)))
    
    # Remove lists from channel field of iovs
    iovs = flatten_channels(iovs)
    
    # Traditional COOL ordering
    iovs.sort(key=lambda iov: (iov.channel, iov.since))

    return IOVSet(iovs)

def connect_adjacent_iovs_defect(generator):
    previous = None
    for iov in generator:
        if (previous and previous.connected_to(iov) and
            previous.comment==iov.comment and previous.channel==iov.channel and
            previous.present==iov.present):
            previous = previous._replace(until=iov.until)
        else:
            if previous:
                yield previous
            previous = iov
    if previous:
        yield previous
    
from subdetector import DCSC_Subdetector, DCSC_DefectTranslate_Subdetector, DCSC_Subdetector_DefectsOnly

from variable import (DCSC_Variable, DCSC_Variable_With_Mapping,
                      DCSC_Global_Variable, DCSC_Defect_Global_Variable)

from subdetector import GoodIOV, OUT_OF_CONFIG
