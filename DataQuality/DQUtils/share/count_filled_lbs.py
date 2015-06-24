#! /usr/bin/env python

from __future__ import division

from DQUtils.db import fetch_iovs, Databases
from DQUtils.oracle import filter_atlas_runs
from DQUtils.sugar import runlumi

from DQUtils.ext.table_printer import pprint_table

from sys import stdout
from pprint import pprint

from PyCool import cool

def sum_lbs(iovs):
    """
    Sum the number of luminosity blocks spanned by a run
    """
    return sum(iov.until - iov.since for iov in iovs)

def fix_iov_lengths(iovs, run_lengths):
    """
    Truncate input iovs so that their length does not exceed the length 
    of the run.
    
    Also clean up records which go from (run, 0) to (run+1, 0)
    """
    def fix_iov(iov):
        """
        Given an iov, correct its since and until if necessary
        """
        run = iov.since.run
        
        if iov.since.run == iov.until.run-1 and iov.until.lumi == 0:
            iov = iov._replace(until=runlumi.make(run, 0xFFFFFFFF))
        
        if run in run_lengths:
            run_length = run_lengths[run]
            if iov.until.lumi > run_length:
                iov = iov._replace(until=runlumi.make(run, run_length))
        
        if iov.since >= iov.until:
            return None
             
        return iov
    
    return [_ for _ in map(fix_iov, iovs) if _ is not None]

def check_bad_iovs(iovs):
    bad_iovs = [iov for iov in iovs 
                if iov.since.run != iov.until.run or iov.until <= iov.since]
                
    if bad_iovs:
        print "%i iovs span runs." % len(bad_iovs)
        pprint(bad_iovs)
        raise RuntimeError, (
            "Hit a bug, bad data on the database. Contact author.")

def fetch_fixedup_folder_information(folder, since, until, cs):
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
    if bad_runs:
        bad_run_count = len(bad_runs)
        print ("  Warning: There are %i runs which don't have a sensible end "
               "time on the\n"
               "  EOR folder, and have been excluded "
               "from the following counts." 
               % bad_run_count)
        print "  They are:",
        print ", ".join(sorted(set("%i" % i.since.run for i in bad_runs)))
        print       
    
    # Remove iovs which go from (run, [max]) to (run+1, 0)
    iovs = [iov for iov in iovs if not 
            (iov.since.lumi == 0xFFFFFFFF and iov.until.lumi == 0)]
    
    # Compute a dictionary of {run: length}, use it to truncate the iovs
    run_lengths = dict((iov.since.run, iov.until.lumi) for iov in info_runs)
    iovs = fix_iov_lengths(iovs, run_lengths)
    
    # Last-ditch check
    check_bad_iovs(iovs)
    
    return iovs, info_runs   

def make_percent(num, denom, decimal_figures=1):
    percent = 100 * num / denom if denom else 0
    return "%.*f%%" % (decimal_figures, percent) if percent else "-"

def generate_lb_count_table(folder, iovs, info_runs): 
    
    runs_by_project_tag = {}
    for iov in info_runs:
        runs_by_project_tag.setdefault(iov.FilenameTag, set()).add(iov.since.run)
    
    table= [
("Project tag", "Runs", "Tot. LBs",   "Fill. LBs", "%",      "Green", "Gr %"),
("",            "",     "EOR", folder,      "Filled", "",      "Total")]     
              
    
    for tag, tag_runs in sorted(runs_by_project_tag.iteritems()):
        if tag == "":
            tag = "<tag not filled>"
            
        tag_info_runs = [iov for iov in info_runs if iov.since.run in tag_runs]
        tag_iovs      = [iov for iov in iovs      if iov.since.run in tag_runs]
        green_iovs    = [iov for iov in tag_iovs  if iov.Code == 3]
        
        runs        = len(set(iov.since.run for iov in tag_info_runs))
        tot_count   = sum_lbs(tag_info_runs)
        iov_count   = sum_lbs(tag_iovs)
        green_count = sum_lbs(green_iovs)
        
        filled_percent = make_percent(iov_count,   tot_count)
        green_percent  = make_percent(green_count, tot_count)
                
        table.append((
            tag, runs, tot_count, iov_count, filled_percent,
            green_count, green_percent
        ))

    tags, runs, tots, filleds, filledps, greens, greenps = zip(*table[2:])    

    table.append((
        "=== Totals ===", sum(runs), sum(tots), 
        sum(filleds), make_percent(sum(filleds), sum(tots)), 
        sum(greens),  make_percent(sum(greens),  sum(tots)),
    ))

    return table

first_cool_use = False
def generate_table(folder, channel, since, until):
    
    cool_folder = Databases.get_folder(folder)
     
    channelid = cool_folder.channelId(channel)
    cs = cool.ChannelSelection(channelid, channelid)
    
    print "Totals for channel '%s' in folder '%s'" % (channel, folder)
    
    iovs, info_runs = fetch_fixedup_folder_information(folder, since, until, cs)
    table = generate_lb_count_table(folder, iovs, info_runs)
    pprint_table(table, header_loc=2)
    
def clear_lookup_line():
    Databases.get_folder("SHIFTOFL"); stdout.write("\x1B[A\x1B[K")

def main():

    clear_lookup_line()
    
    folder = "SHIFTOFL"
    #folder = "DCSOFL"
    since, until = (120000, 0), (200000, 0)
    #cs = cool.ChannelSelection(111, 111)
    channel = "SCTB"
    
    generate_table(folder, channel, since, until)
    
    
if __name__ == "__main__":
    main()
