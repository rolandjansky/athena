# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DQUtils.db import fetch_iovs, Databases, list_to_channelselection
from DQUtils.channel_mapping import convert_channel, channel_mapping
from DQUtils.oracle import filter_atlas_runs
from DQUtils.sugar import runlumi
from DQUtils.iov_truncator import fix_iov_lengths

from DQUtils.detector_hierarchy import detector_hierarchy

from DQUtils.ext.table_printer import pprint_table
from DQUtils.ext.silence import silence

from DQUtils.sugar import runlumi

def sum_lbs(iovs):
    """
    Sum the number of luminosity blocks spanned by a run
    """
    return sum(iov.until - iov.since for iov in iovs)

def check_bad_iovs(iovs):
    bad_iovs = [iov for iov in iovs 
                if iov.since.run != iov.until.run or iov.until <= iov.since]
                
    if bad_iovs:
        logger.debug("%i iovs span runs." % len(bad_iovs))
        logger.debug(bad_iovs)
        raise RuntimeError, (
            "Hit a bug, bad data on the database. Contact author.")

def fetch_fixedup_folder_information(folder, since, until, cs=None, quiet=True):
    """
    Retrieves `iovs` and `info_runs` from COOL.
    
    `iovs` is a list of iovs on folder in the range `since` `until` 
        belonging to `cs`. These iovs are corrected to make up for deficiencies
        in the data on the database.
    
    `info_runs`
    """

    info_runs = fetch_iovs("EOR", since, until, 
                              with_channel=False, what=["FilenameTag"])
    
    iovs = fetch_iovs(folder, since, until, cs, what=["Code"])
    
    # Filter the iovs by those that appear in the ATLAS partition
    info_runs = filter_atlas_runs(info_runs)
    iovs      = filter_atlas_runs(iovs)
    
    bad_runs  = [iov for iov in info_runs if iov.until.lumi == 0xFFFFFFFF]
    info_runs = [iov for iov in info_runs if iov.until.lumi != 0xFFFFFFFF]
    if bad_runs and not quiet:
        bad_run_count = len(bad_runs)
        print ("  Warning: There are %i runs which don't have a sensible end "
               "time on the\n"
               "  EOR folder, and have been excluded "
               "from the following counts." 
               % bad_run_count)
        print "  They are:",
        print ", ".join(sorted(set("%i" % i.since.run for i in bad_runs)))
        print       
    
    # Remove iovs which go from (run, [max]) to (run+1, 0) or (,1)
    iovs = [iov for iov in iovs if not 
            (iov.since.lumi == 0xFFFFFFFF and iov.until.lumi <= 1)]
    
    # Compute a dictionary of {run: length}, use it to truncate the iovs
    run_lengths = dict((iov.since.run, iov.until.lumi) for iov in info_runs)
    iovs = fix_iov_lengths(iovs, run_lengths)
    
    # Last-ditch check
    check_bad_iovs(iovs)
    
    return iovs, info_runs   

def make_percent(num, denom, decimal_figures=1):
    percent = 100 * num / denom if denom else 0
    return "%.*f%%" % (decimal_figures, percent) if percent else "-"
    
def iovs_in_runs(iovs, runs):
    """
    Return a list of `iovs` whos run number is in the set `runs`
    """
    return [iov for iov in iovs if iov.since.run in runs]

def table_header(folder):
    return [
("Project tag", "Runs", "Tot. LBs",   "Fill. LBs", "%",      "Green", "Gr %",  "Black", "Bl %"),
("",            "",     "EOR", folder,      "Filled", "",      "Total", "",      "Total")]

def make_table_row(tag, tag_info_runs, tag_iovs, n_channels):
    """
    Make one row for the information table
    """
    green_iovs    = [iov for iov in tag_iovs  if iov.Code == 3]
    black_iovs    = [iov for iov in tag_iovs  if iov.Code == -1]
    
    runs        = len(set(iov.since.run for iov in tag_info_runs))
    tot_count   = sum_lbs(tag_info_runs)*n_channels
    iov_count   = sum_lbs(tag_iovs)
    green_count = sum_lbs(green_iovs)
    black_count = sum_lbs(black_iovs)
    
    filled_percent = make_percent(iov_count,   tot_count)
    green_percent  = make_percent(green_count, tot_count)
    black_percent  = make_percent(black_count, tot_count)
    
    # gnb = green not black
    gnb_percent    = make_percent(green_count, tot_count-black_count)
            
    return (
        tag, runs, tot_count, iov_count, filled_percent,
        green_count, green_percent, black_count, black_percent, gnb_percent
    )
    
def generate_lb_count_table(folder, iovs, info_runs): 
    """
    Generates a table from a given set of IoVs
    """
    runs_by_project_tag = {}
    for iov in info_runs:
        tag_runs = runs_by_project_tag.setdefault(iov.FilenameTag, set())
        tag_runs.add(iov.since.run)
    
    table = []
    
    n_channels = len(set(iov.channel for iov in iovs))
    
    for tag, tag_runs in sorted(runs_by_project_tag.iteritems()):
        if tag == "":
            tag = "<tag not filled>"
            
        tag_info_runs = iovs_in_runs(info_runs, tag_runs)
        tag_iovs      = iovs_in_runs(iovs,      tag_runs)

        table.append(make_table_row(tag, tag_info_runs, tag_iovs, n_channels))

    (tags, runss, tots, filleds, filledps, greens, greenps, blacks, blackps,
     gnbps,
    ) = zip(*table)    

    table.append((
        "=== Totals ===", sum(runss), sum(tots), 
        sum(filleds), make_percent(sum(filleds), sum(tots)), 
        sum(greens),  make_percent(sum(greens),  sum(tots)),
        sum(blacks),  make_percent(sum(blacks),  sum(tots)),
        make_percent(sum(greens),  sum(tots) - sum(blacks)),
    ))

    return table
    
def test():
    from pprint import pprint
    
    lumi_estimates = fetch_iovs("/TRIGGER/LUMI/LBLESTONL", 
                                   channels=[0], 
                                   database="trigger")
    print lumi_estimates
    #pprint(list(lumi_estimates[-10:]))
    
    iovs = fetch_iovs("/TDAQ/RunCtrl/DataTakingMode", 
                         with_channel=False,
                         database="tdaq")
    
    
    #pprint(list(iovs))
    
    #lumi_estimates = lumi_estimates[:10]
    
    physics_iovs = iovs.select(ReadyForPhysics=1)
    
    print "iovs=", iovs
    print "physics_iovs=", physics_iovs
    from DQUtils.general import timer
    
    from DQUtils.iov_alignment import compute_aligned_iovs, compute_aligned_iovs_with_heap, intersect_iovs
    
    #pprint(list(lumi_estimates))
    #print
    #pprint(list(physics_iovs))
    #print
    
    from time import time
    start = time()
    le, pi = compute_aligned_iovs(lumi_estimates, physics_iovs)
    end = time()
    print "Took %.3f with old algo" % (end-start)
    print le, pi

    #pprint(list(le[:10]))
    #pprint(list(pi[:10]))
        
    from DQUtils.sugar import IOVSet
    from time import time
    start = time()
    #le, pi = map(IOVSet, zip(*list(compute_aligned_iovs_with_heap(lumi_estimates, physics_iovs))))
    le, pi = intersect_iovs(lumi_estimates, physics_iovs)
    end = time()
    print "Took %.3f with heap" % (end-start)
    print le, pi
    
    pprint(list(le[-10:]))
    pprint(list(pi[-10:]))
    
if __name__ == "__main__":
    from IPython.Shell import IPShellEmbed
    ipython_instance = IPShellEmbed(["-pdb"], rc_override=dict(quiet=True))
    test()
