#! /usr/bin/env python
from __future__ import with_statement

from DCSCalculator2.subdetectors import CSC, LAr, Lucid, MDT, Pixels, RPC, SCT, TDQ, Tile, TGC, TRT

from DQUtils.general import timer, kgrind_profile

from DQUtils.db import fetch_iovs
from DQUtils.iov_arrangement import inverse_lblb
from DQUtils.utils import pprint_objects
from DQUtils.sugar import RunLumi

from DQUtils.iov_differences import emit_iov_differences
from DQUtils.lb_counts import sum_lbs

def print_diff_lumi(diff):
    before, after = diff
    assert (before.since, before.until) == (after.since, after.until)
    assert before._fields == after._fields
    
    if not hasattr(type(before), "_is_empty"): type(before)._is_empty = False
    if not hasattr(type(after),  "_is_empty"): type(after)._is_empty = False
    
    print "  ", before.since, before.until,
    
    if before._is_empty:
        print "  Empty -> Filled"
    elif after._is_empty:
        print "  Filled -> Empty"
    else:
        for field, val_before, val_after in zip(before._fields, before, after):
            if val_before != val_after:
                print field, val_before, "->", val_after,
        print

def compare_iovs(a, b):
    
    a = a.by_channel
    b = b.by_channel
    
    for output_channel in sorted(set(a.keys() + b.keys())):
        A = a[output_channel]
        B = b[output_channel]
        
        diff = emit_iov_differences(A, B)
        print " [BAD] diff iovs =", len(diff), "lbs =", sum_lbs(zip(*diff)[0])
        print "  First ten diffs:"
        for i in diff[:10]:
            #print i
            print_diff_lumi(i)
        print        

def main():
    run = 144247 # Recent run where CSC gas state was bad.
    run = 136379
    run = 155112 # Recent long run (Sat May 15th, 11am -> Sun 16th 8am), 632 lbs
    run = 156463
    #run = 155669 # MDT NConfig goes below 600
    from sys import argv
    args = argv[1:]
    if args:
        run = int(args[0])
    since, until = RunLumi(run, 0), RunLumi(run+1, 0)

    with timer("Read LBLB"):
        lblb = fetch_iovs("LBLB", since, until, with_channel=False)
        lbtime = inverse_lblb(lblb)
    
    #system = SCT() # Works, very slightly discrepant for the config.
    #system = Pixels() # Works
    #system = Tile() # Works
    #system = CSC() # Needs globals fixing
    #system = TDQ() # Works, needs further testing
    #system = RPC() # Works
    #system = LAr() # Works, but produces slightly different result. Original gives: 
                    # DCSOFL_VAL(since=(155112:  167), until=(155112:  176), channel=215, Code=2L)
                    
    #system = TRT() # Unresolved discrepancies: Wrong NConfig, NWorking off by one in run 155112.
    
    system = MDT() # Seems to work
    #system = Magnets() # Needs to be finished
    #system = Lucid() # Needs to be written
    
    #print system.mapping.keys()
    
    with timer("Calculate result for %s" % system):
        result = system.run(lbtime)
    
    print "Done."
    print
    #print result[0]
    
    print "Run range:", since, "to", until
    print "DCSC2 intervals of validity:"
    pprint_objects(result[:30])
    print
    
    output_channels = system.mapping.keys()
    iovs = fetch_iovs("DCSOFL", since, until, output_channels)
    
    #print hex(hash(tuple(iovs))), hex(hash(tuple(result)))
    
    print "Original DCSC intervals of validity:"
    pprint_objects(iovs[:30])
    
    return
    
    compare_iovs(iovs, result)
    
    #for calc_iov, ref_iov in zip(result, iovs):
        #print calc_iov.since, calc_iov.until, ref_iov.since, ref_iov.until
    #pprint_objects(result)
    
if __name__ == "__main__":

    from IPython.Shell import IPShellEmbed
    ipython_instance = IPShellEmbed(["-pdb"], rc_override=dict(quiet=True))
    
    from DQUtils.logger import init_logging
    init_logging()
    
    main()
