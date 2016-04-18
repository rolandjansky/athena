# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DQUtils import fetch_iovs, process_iovs, IOVSet

class UnknownTrigger(RuntimeError):
    "Raised when an unknown trigger is requested for a time interval"

LVL1_MENU = "COOLONL_TRIGGER::/TRIGGER/LVL1/Menu"

def get_trigger_channel(trigger, iovrange):
    """
    Returns the set of iovs for which the trigger is valid
    """
    
    raise NotImplementedError("Implementation incomplete")
    
    # First check just the first run.
    iovs = fetch_iovs(LVL1_MENU, runs=iovrange.since.run, what=["ItemName"])

    result = [i for i in iovs if i.ItemName == trigger]
    if not result:
        raise UnknownTrigger()
    
    assert len(result) == 1, "This should not happen."
    
    channel = result[0].channel
    
    iovs = fetch_iovs(LVL1_MENU, channels=[channel], what=["ItemName"], *iovrange)
    
    return iovs.select(ItemName == trigger)

def compute_lumi(lbs, lumis, iovs, exclude_iovsets=[], good_runs=None):
    """
    Compute luminosity for an IOVSet iovs using lbs, lumis from fetch_lumi_inputs.

    exclude_iovsets is a list of IOVSets for channels to be excluded from the 
    calculation such as not_ready, busy, etc.

    good_runs is an optional set of runs to consider, such as the set of runs
    which are OK for physics.
    """
    # This is the old simple approach, which doesn't have a lot of features
    #inputs = process_iovs(lbs, lumis, iovs)
    #return sum((lb.EndTime - lb.StartTime)/1e9 * lumi.LBAvInstLumi 
               #for since, until, (lb, lumi, good) in inputs
               #if lb and lumi and good)
    
    inputs = process_iovs(lbs, lumis, iovs, *exclude_iovsets)
    total_lumi = 0
    for since, until, states in inputs:
        lb, lumi, good, excludes = states[0], states[1], states[2], states[3:]

        # Skip this iov if requested info not available 
        # or if any excluded channels are present
        if not lb or not lumi or not good or any(excludes):
            continue

        # Optional set of runs to consider.  Reject if not in set
        if good_runs and since.run not in good_runs:
            continue
        
        total_lumi += (lb.EndTime - lb.StartTime)/1e9 * lumi.LBAvInstLumi

    return total_lumi

def compute_lumi_many_channels(lbs, lumis, iovs, exclude_iovsets=[], good_runs=None):
    """
    Compute luminosity per channel for an IOVSet iovs using 
    lbs, lumis from fetch_lumi_inputs.

    exclude_iovsets is a list of IOVSets for channels to be excluded from the 
    calculation such as not_ready, busy, etc.

    good_runs is an optional set of runs to consider, such as the set of runs
    which are OK for physics.

    Return type is a dictionary of {channel name: lumi}
    """
    # Split the IOVSet by channel
    chans, iovsets = iovs.chans_iovsets
    result = {}

    # Old method, keeping it here for now
    #inputs = process_iovs(lbs, lumis, *iovsets)
    #for since, until, states in inputs:
        #(lb, lumi), defects = states[:2], states[2:]
        #if not (lb and lumi and lumi.LBAvInstLumi):
            #continue
        #lumi = (lb.EndTime - lb.StartTime)/1e9 * lumi.LBAvInstLumi 
        #for name, defect_iov in zip(chans, defects):
            #if defect_iov:
                #result[name] = result.get(name, 0) + lumi

    # New approach, adding new functionality
    num_exclude = len(exclude_iovsets)
    full_iovsets = exclude_iovsets
    full_iovsets.extend(iovsets)
    inputs = process_iovs(lbs, lumis, *full_iovsets)
    for since, until, states in inputs:
        # I find the below version conceptually cleaner
        #(lb, lumi), excludes, defects = (states[:2], states[2:2+num_exclude], 
                                         #states[2+num_exclude:])
        (lb, lumi), defectstates = states[:2], states[2:]
        excludes, defects = defectstates[:num_exclude], defectstates[num_exclude:]

        # Skip this iov if requested info not available 
        # or if any excluded channels are present
        #if not (lb and lumi and lumi.LBAvInstLumi):
        if not lb or not lumi or any(excludes):
            continue

        # Optional set of runs to consider.  Reject if not in set
        if good_runs and since.run not in good_runs:
            continue
        
        # Lumi for this lb
        lumi = (lb.EndTime - lb.StartTime)/1e9 * lumi.LBAvInstLumi 
        # Loop over channels present, add lumi
        for name, defect_iov in zip(chans, defects):
            if defect_iov:
                result[name] = result.get(name, 0) + lumi

    return result

def fetch_lumi_inputs(range_iov, tag="OflLumi-7TeV-002"):
    """
    Retrieve information required to calculate the luminosity from the database
    """
    lbs = fetch_iovs("LBLB", with_channel=False, #loud=True,
                     *range_iov)
    
    if tag == "ONLINE":
        lumis = fetch_iovs("COOLONL_TRIGGER::/TRIGGER/LUMI/OnlPrefLumi",
                           channels=[0], #loud=True,
                           *range_iov)
        # Select valid
        lumis = IOVSet(l for l in lumis if (l.Valid & 0xFFFF) % 10 == 0)
    else:
        # Apparently this string changes from time to time...
        lumi_folder = "COOLOFL_TRIGGER::/TRIGGER/OFLLUMI/OflPrefLumi" # Run-2
        #lumi_folder = "COOLOFL_TRIGGER::/TRIGGER/OFLLUMI/LBLESTOFL" # Run-1
        lumis = fetch_iovs(lumi_folder,
                           tag=tag,
                           channels=[0], #loud=True,
                           *range_iov)

    return lbs, lumis    
