# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from .sugar import IOVSet, TimestampType, define_iov_type
from collections import defaultdict

@define_iov_type
def LBTIME_VAL(Run, LumiBlock):
    pass

@define_iov_type
def RUNIOV_VAL():
    pass

def run_iovs_from_lblb(lblb):
    gen = (RUNIOV_VAL(lb.since, lb.until) for lb in lblb)
    result = IOVSet(connect_adjacent_iovs(gen))
    return result
    #return IOVSet(iov._replace(until=iov.until+1) for iov in result)
    
def split_by_channel(iovs):
    """
    Assumes `iovs` ordered by channel, then by since.
    """
    empty_iovset_maker = iovs.empty_maker()
    result = defaultdict(lambda: empty_iovset_maker())
    last_channel = None
    for iov in iovs:
        if iov.channel != last_channel:
            last_channel = iov.channel
            appender = result[last_channel].append
        appender(iov)
    return result
    
def inverse_lblb(iovs):    
    @staticmethod
    def from_LBLB_VAL(lblb):
        return LBTIME_VAL(TimestampType(lblb.StartTime), 
                          TimestampType(lblb.EndTime), 
                          lblb.since.run, lblb.since.lumi)
    
    LBTIME_VAL.from_LBLB_VAL = from_LBLB_VAL
    
    lbtime_iovs = IOVSet(LBTIME_VAL.from_LBLB_VAL(iov) for iov in iovs)
    
    if sorted(lbtime_iovs) != lbtime_iovs:
        raise RuntimeError("lblb records are broken")

    return lbtime_iovs

def add_result_iov(iov, output_iovs):
    """
    Consider a single result `iov`, and connect it to existing result iovs
    before it if necessary
    
    Also performs translation of channel names to channel IDs.
    """
    last_iov = output_iovs[-1] if output_iovs else None
    
    if last_iov and last_iov.connected_to(iov):
        # It is connected to the last IoV, we can just update the last extent
        output_iovs[-1] = last_iov._replace(until=iov.until)
    
    else:
        output_iovs.append(iov)

def connect_adjacent_iovs(generator):
    previous = None
    for iov in generator:
        if previous and previous.connected_to(iov):
            previous = previous._replace(until=iov.until)
        else:
            if previous:
                yield previous
            previous = iov
    
    if previous:
        yield previous

def flatten_channels(iovs):
    """
    Input/Output: list of iovs
    
    If an iov specifies an iterable for its channel, then expand it into 
    a list of channels.
    """
    from six import string_types
    result = iovs.empty()
    for iov in iovs:
        if not isinstance(iov.channel, string_types) and hasattr(iov.channel, "__iter__"):
            for c in iov.channel:
                result.append(iov._replace(channel=c))
        else:
            result.append(iov)
            
    return result
