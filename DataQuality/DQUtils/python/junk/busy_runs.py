#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
A "busy" run is one in which lots of things happen.

This script is intended to identify runs where lots of iovs occured. These are
presumably good for testing software that generated them, since we expect
to reproduce the same result.
"""

MAX = 2**32 - 1 

from DQUtils.db import fetch_iovs
from DQUtils.sugar import runlumi
from DQUtils.iov_arrangement import split_by_channel

from DQUtils.channel_mapping import channel_mapping

from PyCool import cool

from collections import defaultdict

def iov_count_per_run(iovs):
    """
    Returns a dictionary {run number: number of iovs}
    """
    result = defaultdict(int)
    
    for iov in iovs:
        result[iov.since.run] += 1
    
    return dict(result)

def fetch_busy_runs(folder, 
                    since=runlumi(0), 
                    until=runlumi(2**63-1),
                    cs=cool.ChannelSelection(), 
                    ntop=5):
    """
    `folder` to query
    `since` start time (defaults to 0)
    `until` until (defaults to iovmax)
    `cs` channelselection (defaults to all)
    `ntop` number of the busyiest runs to retrieve (defaults to 5)
    """
    
    result = []
    
    print "Fetching iovs.."
    iovs = split_by_channel(fetch_iovs(folder, since, until, cs, loud=True))
    
    print "Tallying by channel.."
    run_counts = {}
    for channel, iovs in iovs.iteritems():
        run_counts[channel] = iov_count_per_run(iovs)
    
    print "Getting busiest.."
    for channel, runs_iov_counts in run_counts.iteritems():
        run_counts[channel] = sorted(runs_iov_counts.items(), 
                                     key=lambda (run, count): (count, -run),
                                     reverse=True)[:ntop]
        
    return sorted(run_counts.iteritems())
    
def test():
    result = fetch_busy_runs("DCSOFL") #, runlumi.make(145000, 0), 
                                       #runlumi.make(146000, 0))
    
    for channel, runcounts in result:
        counts = " | ".join(["%6i : %4i" % (r, c) for r, c in runcounts[:3]])
        print "%3i %5s - %s" % (channel, channel_mapping[channel], counts)
        
if __name__ == "__main__":
    test()
