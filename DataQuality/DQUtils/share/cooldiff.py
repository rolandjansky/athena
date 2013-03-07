#! /usr/bin/env python
"""
An example script for finding the difference between tags/folders
"""

from DQUtils import fetch_iovs, process_iovs
from DQUtils.sugar import IOVSet

def make_pretty(iov):
    return "[Empty]" if iov is None else iov._contents

def diff_channel(channel, iovs_1, iovs_2):

    for since, until, (iov_1, iov_2) in process_iovs(iovs_1, iovs_2):
        
        if until.lumi == 0 or since.lumi == 0xFFFFFFFF:
            # Kill silly IoV ranges
            continue
            
        if iov_1 is None and iov_2 is None:
            # Both are empty
            continue
        
        if iov_1 is None or iov_2 is None:
            # One or the other is empty
            print "  %r to %r:" % (since, until), make_pretty(iov_1), make_pretty(iov_2)
            continue
        
        elif iov_1._contents == iov_2._contents:
            # They are both equal (no difference)
            continue
    
        # There is a difference.
        print "  %r and %r:" % (since, until),
        print "%s => %s" % (iov_1.Code, iov_2.Code)

def diff_folder(iov_range, folder, tag_1, tag_2, interesting_channels):
    other_args = dict(named_channels=True)
    iovs_1 = fetch_iovs(folder, tag=tag_1, *iov_range, **other_args).by_channel
    iovs_2 = fetch_iovs(folder, tag=tag_2, *iov_range, **other_args).by_channel
    
    all_channels = set(iovs_1.keys() + iovs_2.keys())
    for channel in sorted(all_channels):
        if channel not in interesting_channels:
            continue
        
        print "Considering", channel
        iovs_1_c = iovs_1.get(channel, IOVSet())
        iovs_2_c = iovs_2.get(channel, IOVSet())
        
        diff_channel(channel, iovs_1_c, iovs_2_c)
        
def main():
    pass
    
def test():
    #folder = "SHIFTOFL"
    folder = "LBSUMM"
    tag = "DetStatusLBSUMM-pass1-analysis-2010B"
    
    iov_range_C = (155228, 0), (156682, 0)
    iov_range = (153565, 0), (155160, 0)
    
    interesting_channels = "TRTB", "TRTEA", "TRTEC"
    
    diff_folder(iov_range, folder, "HEAD", tag, interesting_channels)
    
if __name__ == "__main__":
    test()
