#! /usr/bin/env python

from __future__ import with_statement

from DQUtils.db import fetch_iovs, Databases
from DQUtils.iov_alignment import compute_unfilled_blocks
from DQUtils.sugar import RunLumi
from DQUtils.oracle import fetch_atlas_runs

from textwrap import wrap
from sys import stdout

def get_run_info(run):
    """
    Return number of LBs, the start and end times of a run.
    """
    iovs = fetch_iovs("LBLB", (run, 0), (run+1, 0))
    if not iovs: return (None, None, None)
    return (iovs.last.until.lumi, RunLumi(iovs.first.StartTime), 
                                  RunLumi(iovs.last.EndTime))

def search_unfilled_lbs(folder, start, end, all_channels):
    """
    Return a list of IoVs which are not filled for a folder
    """
    all_iovs = fetch_iovs(folder, start, end)
    return all_iovs, compute_unfilled_blocks(start, end, all_channels, all_iovs)

def search_run(folder, all_channels, run):
    """
    Search one run for unfilled lbs.
    """

    lb_count, start, end = get_run_info(run)
    if not lb_count:
        print "- No LBs for run"
        return None, None
    
    if end - start > (24*3600*4 * 1e9):
        print "query too long, skipping. (> 4 days)"
        return None, None

    args = folder, start, end, all_channels
    return search_unfilled_lbs(*args)

def run_tests(database, test_list):
    
    for folder_name, runs_to_test in test_list:
    
        folder = Databases.get_folder(folder_name, database)
        all_channels = set(folder.listChannels())
        
        print "Querying folder", folder_name
        
        for run in runs_to_test:
            all_iovs, unfilled_iovs = search_run(folder, all_channels, run)
            if all_iovs is None: continue
            
            if unfilled_iovs:
                args = (run, lb_count, start, end)
                print "  Run: %6i LBs: %4i from: %s to: %s" % args
                print "  --> Total IoVs:", all_iovs
                print "  --> Empty IoVs:", unfilled_iovs
                channels = sorted(unfilled_iovs.channels)
                channel_text = "\n".join(wrap(repr(channels), 
                                              subsequent_indent=" "*19))
                print "  Channels (N=%i) %s" % (len(channels), channel_text)
                print "  -- First five iovs:"
                from pprint import pprint
                pprint(list(unfilled_iovs)[:5])
        
        print
        print

def main():
    
    # List of folders and run-numbers to investigate
    test_list = [
        ("/SCT/DCS/HV", [138790]),
        ("/LAR/DCS/FSM", [121416]),
    ]
    
    database = "COOLOFL_DCS/COMP200"
    
    # Set me to true to run the above
    run_tests = False
    
    if run_tests:
        run_tests(database, test_list)
        return
    
    #folder_name = "/SCT/DCS/HV"
    folder_name = "/LAR/DCS/FSM"
    
    folder = Databases.get_folder(folder_name, database)
    all_channels = set(folder.listChannels())
    
    atlas_runs = list(zip(*fetch_atlas_runs())[0])
    
    first = atlas_runs.index(121416) #138790)
    last = first + 100
    
    print "Entering loop"
    
    for run in atlas_runs[first:last]:
    
        print "Testing run:", run,
        stdout.flush()
        
        all_iovs, unfilled_iovs = search_run(folder, all_channels, run)
        if all_iovs is None: continue
        
        if unfilled_iovs:
            print "- contains unfilled iovs:" , unfilled_iovs,
        else:
            print "- OK",
            
        print
                
if __name__ == "__main__":
    profiling = False
    if profiling:
        from DQUtils.general import kgrind_profile
        with kgrind_profile() as p:
            p.run("main()")
    else:
        main()
