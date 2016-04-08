#! /usr/bin/env python

from DQUtils import fetch_iovs, IOVSet, process_iovs
from DQUtils.channel_mapping import list_to_channelselection

from DCSCalculator2.lib import DCSC_Global_Variable

def pretty_range(r):
    a, b = r
    if a == b:
        return "%i" % a
    return "%i..%i" % r

def pretty_ranges(channels):
    channels = list_to_channelselection(channels, as_list=True, convert_channel=None)
    return ", ".join(pretty_range(r) for r in channels)
    
def show_variable_info(lbtime, Subdetector):
    subdetector = Subdetector()
    for variable in subdetector.variables:
        print "Processing variable:", variable.folder_name
        good_iovs = variable.calculate_good_iovs(lbtime, subdetector).iovs
        
        chans, iovs = good_iovs.chans_iovsets
            
        if isinstance(variable, DCSC_Global_Variable):
            
            for since, until, states in process_iovs(*iovs):
                channels = [(state.channel, state.Code) for state in states 
                            if state and state.Code < 3]
                
                print since, until, pretty_ranges(channels)
        else:
            
            for since, until, states in process_iovs(*iovs):
                channels = [state.channel for state in states 
                            if state and not state.good]
                print since, until, pretty_ranges(channels)

def main():
    from DCSCalculator2.subdetectors import CSC, MDT

    lbtime = fetch_iovs("LBTIME", (177540, 0))

    print "lbtime =", lbtime

    show_variable_info(lbtime, MDT)

if __name__ == "__main__":
    main()
