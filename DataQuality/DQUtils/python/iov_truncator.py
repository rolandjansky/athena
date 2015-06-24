#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from __future__ import with_statement
from collections import namedtuple
from os import environ

from .oracle import atlas_runs_set
from .general import timer
from .sugar import RunLumi
from .general import flattened
from .events import iov_yielder

try:
    from .db import fetch_iovs
except ImportError:
    def rlumi(run, lumi):
        return run << 32 | lumi

    def un_rlumi(x):
        return x >> 32, x & 0xFFFFFFFF
        
    def fetch_iovs(*args, **kwargs):
        """
        A dummy fetch_iovs for testing. Returns two runs, one 100 LBs long 
        and the other 50 long.
        """
        from collections import namedtuple
        T = namedtuple("EOR_VAL", "since, until RunNumber")
        
        return map(T._make, [(rlumi(  1, 0), rlumi(1,  100),           1),
                             (rlumi(  2, 0), rlumi(2,   50),           2),
                             (rlumi(100, 0), rlumi(100, 10),         100),
                             (rlumi(101, 0), rlumi(101, 0xFFFFFFFF), 101)])

def fix_iov_lengths(iovs, run_lengths):
    """
    Truncate input iovs so that their length does not exceed the length 
    of the run.
    
    Also clean up records which go from (run, 0) to (run+1, 0)
    
    `run_lengths` should be a dictionary {run_number: lumiblock count}
    """
    def fix_iov(iov):
        """
        Given an iov, correct its since and until if necessary
        """
        run = iov.since.run
        
        if iov.since.run == iov.until.run-1 and iov.until.lumi == 0:
            iov = iov._replace(until=RunLumi(run, 0xFFFFFFFF))
        
        if run in run_lengths:
            run_length = run_lengths[run]
            if iov.until.lumi > run_length:
                iov = iov._replace(until=RunLumi(run, run_length))
        
        if iov.since >= iov.until:
            return None
             
        return iov
    
    return [_ for _ in map(fix_iov, iovs) if _ is not None]

def make_run_iovs(iovs):
    # Find the minimum and maximum extent for this set of iovs
    since = min(min(o.since for o in objects) for objects in iovs)
    until = max(max(o.until for o in objects) for objects in iovs)
    
    run_iovs = fetch_iovs("EOR", since, until, 
                             with_channel=False, what=[])
    
    atlas_runs = atlas_runs_set()
    
    run_iovs = (iov for iov in run_iovs if iov.since.run in atlas_runs)
    return run_iovs

def truncate_to_atlas_runs(*iovs):
    """
    Given a set of iovs
    """
    
    run_iovs = make_run_iovs(iovs)
    return truncate_to_run_iovs(run_iovs, *iovs)
    
def truncate_to_run_iovs(run_iovs, *iovs):
    result_iovs = [[] for i in iovs]
    active_states, ended_states = [set() for i in iovs], [set() for i in iovs]
    
    def bind_to_run(run_iov, active_states):
        """
        Given a run_iov and a set of active iovs, emit iovs which are bound
        to this run.
        """
        for active_state in sorted(active_states):
            iov = active_state._replace(
                since=max(run_iov.since+1, active_state.since),
                until=min(run_iov.until,   active_state.until))
                
            if iov.since != iov.until:
                # Prevent zero-length iovs from being emitted
                yield iov
    
    for position, index, beginning, iov in iov_yielder(run_iovs, *iovs):
        if index == 0:
            # Run events
            if beginning:
                # Run start
                # * Invalidate ended iovs
                # * Clear sets of ended iovs
                for active, ended in zip(active_states, ended_states):
                    active.difference_update(ended)
                    ended.clear()
            else:
                # Run end - Intersect active iovs for each channel with this run
                #         - Results are stored
                for i, chan_active_states in enumerate(active_states):
                    result_iovs[i].extend(bind_to_run(iov, chan_active_states))
        else:
            # IOV events
            index -= 1
            # Add current iov to active states if it has just started, otherwise
            # ended states.
            action = active_states[index] if beginning else ended_states[index]
            action.add(iov)
            
    return result_iovs
    
def test_truncator():
    
    from collections import namedtuple
    IOV = namedtuple("IOV", "since until channel state")
    GOOD = True
    BAD = False
    A, B, C = 1, 2, 3
    
    def __repr__(self):
        """
        A pretty convertor for (since, until)
        """
        args = un_rlumi(self.since) + un_rlumi(self.until) + (
                self.channel, self.state)
                
        return "IOV(since=(%i, %i), until=(%i, %i), channel=%i, state=%s)" % args
    IOV.__repr__ = __repr__
    
    iovs = [
        IOV(0, 1, A, GOOD),
        IOV(rlumi(  0, 0), rlumi(2**31, 0), A, GOOD),
        IOV(rlumi(  0, 0), rlumi(100,   0), B, GOOD),
        IOV(rlumi(100, 0), rlumi(2**31, 0), B, BAD),
        IOV(rlumi(101, 0), rlumi(101, 100), A, GOOD),
        IOV(rlumi(101, 0), rlumi(101, 0),   B, BAD),
    ]
    
    result = truncate_iovs(iovs)
    
    print len(result)
    
    from pprint import pprint
    
    pprint(result)
    
if __name__ == "__main__":
    
    test_truncator()
