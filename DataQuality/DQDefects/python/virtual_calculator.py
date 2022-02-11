# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from logging import getLogger

from .virtual_logic import DefectLogic; log = getLogger("DQDefects.virtual_defect_calculator")

from collections import defaultdict

from DQUtils import process_iovs_changed
from DQUtils.sugar import IOVSet, RunLumi

from DQDefects import DEFECT_IOV

import six

from collections.abc import Container
from typing import Tuple, Union, Optional, Iterable, Generator, Mapping


def generate_virtual_defects(by_channel: Mapping[str, IOVSet], logics: Iterable[DefectLogic], 
                             since_cr: Union[int, Tuple[int, int], RunLumi], 
                             until_cr: Union[int, Tuple[int, int], RunLumi], 
                             ignore: Optional[Container[str]]
                            ) -> Generator[Tuple[RunLumi, RunLumi, Mapping[str, DEFECT_IOV]], None, None]:
    """
    An iterator which emits (since, until, {channel name : iov})
    
    It also computes the state of the virtual flags for each (since, until)
    region.
    """    
    all_channels = list(by_channel.keys()) + [l.name for l in logics]
    states = dict((channel, None) for channel in all_channels)
    
    channels, iovsets = zip(*sorted(six.iteritems(by_channel)))
        
    for since, until, current_iovs, changes in process_iovs_changed(*iovsets):
        # Update things that changed since last iteration in the states dict
        for change in changes:
            if ignore is not None and channels[change] in ignore:
                continue
            states[channels[change]] = current_iovs[change]
        
        # If we're completely outside the calculation range, don't yield iovs
        if until <= since_cr or since >= until_cr:
            continue
            
        # Update flag calculation result
        for logic in logics:
            if ignore and logic.name in ignore:
                continue
            states[logic.name] = logic.evaluate(states)
        
        yield since, until, states
        
def bad_iov(since: RunLumi, until: RunLumi) -> bool:
    """
    Skip some commonly emitted nonsensical IoVs.
    """
    return (
        # Skip 1-lb long iovs on run borders
        (until - since == 1 and since.run != until.run) or
        # Skip LB 0->1 IoVs
        (since.lumi == 0 and until.lumi == 1 and since.run == until.run)
    )

def calculate_virtual_defects(primary_iovs: IOVSet, evaluation_order: Iterable[DefectLogic],
                              virtual_output_channels: Iterable[str], primary_output_channels: Iterable[str], 
                              since: Optional[Union[int, Tuple[int, int], RunLumi]], 
                              until: Optional[Union[int, Tuple[int, int], RunLumi]], 
                              ignore: Optional[Container[str]]) -> IOVSet:
    """
    Returns a list of IoVs for a given query in the normal COOL order
    (sorted by channelID, then by since)
    """
    
    if since is None: since = 0
    if until is None: until = 2**63-1
    since, until = RunLumi(since), RunLumi(until)
    
    result = defaultdict(IOVSet)
    
    primary_by_channel = primary_iovs.by_channel
    
    # Copy desired primary channels to the result
    for primary_channel, primary_iovs in six.iteritems(primary_by_channel):
        if primary_channel in primary_output_channels:
            if ignore is not None and primary_channel in ignore:
                continue
            result[primary_channel] = primary_iovs
    
    args = primary_by_channel, evaluation_order, since, until, ignore

    # Skip vfgen loop if there is no virtual logic to compute
    vfgen = [] if not evaluation_order else generate_virtual_defects(*args)
    
    for since, until, virtual_iovs in vfgen:
        if bad_iov(since, until):
            continue
            
        for output_name in virtual_output_channels:
            # Consider the state of each desired output for this 
            # (since, until) and write it to output_iovs
            iov = virtual_iovs.get(output_name)
            if iov and iov.present:
                result[output_name].add(since, until, *iov[2:])
    
    # Sort them by traditional COOL sort ordering (by channelId first, 
    # then by since. `iovs` are already ordered by since.)
    result_list = IOVSet()
    for _, iovs in sorted(six.iteritems(result)):
        result_list.extend(iovs.solidify(DEFECT_IOV))
        
    return result_list
