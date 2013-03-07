#! /usr/bin/env python
"""
Compute quantites such as minimum, maximum and average dead fraction for a run

Peter Waller <peter.waller@cern.ch> June 2010
"""

from DQUtils import fetch_iovs, process_iovs
from DQUtils.grl import grl_iovs_from_xml
from DQUtils.sugar import IOVSet, define_iov_type

from collections import namedtuple
from pprint import pprint

RunInfo = namedtuple("RunInfo", "lowest average highest")

def compute_min_max_avg(iovs, quantity):
    """
    Compute minimum, maximum and weighted average over a set of `iovs` for a given 
    `quantity_name` (such as "dead_fraction" or "NConfig")
    """
    getter = lambda i: getattr(i, quantity)
    
    lowest = min(getter(iov) for iov in iovs)
    highest = max(getter(iov) for iov in iovs)
    
    lb_count = sum(iov.length for iov in iovs)
    weighted_sum = sum(getter(iov) * iov.length for iov in iovs)
    average = weighted_sum / lb_count
    
    return lowest, average, highest

def dcs_summary(grl_iovs, dcs_iovs, quantity="dead_fraction"):
    """
    Calculate minimum, maximum and weighted average for `quantity` from 
    the `dcs_iovs` iov sets, for lumiblocks within the `grl_iovs`.
    """
    
    channels, dcs_iovs = zip(*sorted(dcs_iovs.by_channel.iteritems()))
    
    # Somewhere to store IoVs which lie within a GRL
    iov_sets = dict((channel, IOVSet()) for channel in channels)
    
    # Fetch iov information from dcs_iovs within a GRL
    # process_iovs pulls out common (since, until, state) efficiently, where
    # `state` is a list of IoVs which are currently active, and in the same 
    # order as the iov sets which are passed to it.
    for since, until, iovs in process_iovs(grl_iovs, *dcs_iovs):
        grl_iov, dcs_states = iovs[0], iovs[1:]
                
        if grl_iov is None:
            # `grl_iov` unset? => We are outside a GRL IoV
            continue
        
        # Copy dead fraction information for iovs lying within a GRL
        for channel, dcs_iov in zip(channels, dcs_states):
            properties = dcs_iov.deadFrac, dcs_iov.NConfig
            iov_sets[channel].add(since, until, *properties)
    
    @define_iov_type
    def DCS_IOV(dead_fraction, n_in_config):
        "Represent a dead fraction"
        
    # 'solidify' required once an iov_set has been filled
    iov_sets = dict((channel, iov_set.solidify(DCS_IOV))
                    for channel, iov_set in iov_sets.iteritems())
    
    result = {}
    
    # Consider channels, then for a given channel consider all runs.
    for channel in channels:
        for run, iovs in sorted(iov_sets[channel].by_run.iteritems()):
            lowest, average, highest = compute_min_max_avg(iovs, quantity)
            run_state = RunInfo(lowest, average, highest)
            result.setdefault(channel, []).append((run, run_state))

    return result
        
def test():
    
    path = "/afs/cern.ch/user/b/beate/public/DQAna/StableBeams-periodC1.xml"
    grl_iovs = grl_iovs_from_xml(path)
    since, until = grl_iovs.range_iov
    dcs_iovs = fetch_iovs("DCSOFL", since, until, named_channels=True)
    
    # Instead of "dead_fraction", "n_in_config" can be used.
    result = dcs_summary(grl_iovs, dcs_iovs, "dead_fraction")
    
    for channel, states in sorted(result.iteritems()):
        print channel
        for run, info in states:
            print "", run, "Low: % .3f Ave: % .3f Hi: % .3f" % (info.lowest, info.average, info.highest)

if __name__ == "__main__":
    test()
