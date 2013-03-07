#! /usr/bin/env python

from os.path import basename, isdir, exists
from DQUtils import IOVSet, process_iovs, define_iov_type
from DQUtils.grl import  grl_from_dir, grl_iovs_from_xml

@define_iov_type
def GRLSTATE_VAL(present_grls, absent_grls):
    """
    Maintains a list of present/absent grls for a given iov range
    """
    
def trunc_left(name):
    return "..%s" % name[-19:]

def compute_grl_diff(inputs, *iovsets):
    """
    Returns a list of IoVs which differ for GRLs
    """    
    result = IOVSet()
    for since, until, grl_states in process_iovs(*iovsets):
        has_difference = any(grl_states) != all(grl_states)
        if has_difference:
            set_iovs = [d for d, state in zip(inputs, grl_states) if state]
            unset_iovs = [d for d, state in zip(inputs, grl_states) if not state]
            result.add(since, until, set_iovs, unset_iovs)

    return result.solidify(GRLSTATE_VAL)

def print_diff(diff):    
    # Dictionary of lb counts per run
    lbs_by_run = dict((run, lbs.lb_counts) 
                      for run, lbs in diff.by_run.iteritems())

    last_run = None
    for since, until, present, absent in diff:
        if last_run != since.run:
            last_run = since.run
            args = last_run, lbs_by_run[last_run]
            print "---- Run: %i (%6i differing lbs) ----" % args
        args = since.lumi, until.lumi, ", ".join(present), ", ".join(absent)
        print "%5s -> %5s | present: %s | absent: %s" % args

def main(inputs):
    if not inputs:
        raise RuntimeError("Please specify a list of directories to compare "
                           "on the command line")
    
    for i in inputs:
        exist = exists(i) if i.endswith(".xml") else isdir(i)
        if not exist:
            raise RuntimeError("Can't find input file/directory named %s" % i)
    
    # Load an iovset per GRL
    iovsets = [IOVSet.from_grl(i) if i.endswith(".xml") else grl_from_dir(i)
               for i in inputs]
    short_inputs = map(trunc_left, inputs)
    
    differences = compute_grl_diff(short_inputs, *iovsets)
    if differences:
        print_diff(differences)
        
        print "Files Checked:"
        for filename, iovs in zip(inputs, iovsets):
            print "  Present LBs: %6i (%s)" % (iovs.lb_counts, filename)
        runs, lbs = len(differences.runs), differences.lb_counts
        print "Total runs (lbs) with differing states: %i (%i)" % (runs, lbs)
    else:
        print "GRLs are equal: "
        for filename in inputs:
            print filename
        
    raise SystemExit(bool(differences))

if __name__ == "__main__":
    from sys import argv
    if "--example" in argv:
        eg_grl_dir = "/afs/cern.ch/atlas/groups/EGamma/EGammaGRL/"
        args = [eg_grl_dir + "eg_forward", eg_grl_dir + "eg_standard"]
        from pipes import quote
        print "Running:", argv[0], " ".join(map(quote, args))
        main(args)
    else:
        main(argv[1:])

