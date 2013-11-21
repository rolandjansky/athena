#! /usr/bin/env python

#from IPython.Shell import IPShellEmbed; ip = IPShellEmbed(["-pdb"])
from DQDefects import DefectsDB
from DQUtils import IOVSet, process_iovs, define_iov_type
from DQUtils.sugar import RunLumi, RANGEIOV_VAL
from DQUtils.periods import fetch_project_period_runs

@define_iov_type
def DEFECTSET_VAL(defects):
    "A set of defect names present for each IoV"
    
@define_iov_type
def DEFECTDIFF_VAL(tag1, tag2):
    "Lists of defects only present in one tag or the other"

def make_defectset_iovs(range_iovs, defect_iovs):
    """
    Return a list of iovs with a set for each IoV containing the present defects
    """
    chans, iovsets = defect_iovs.chans_iovsets
    
    result = IOVSet()
    for since, until, states in process_iovs(range_iovs, *iovsets):
        in_range, defects = states[0], states[1:]
        if in_range:
            result.add(since, until, set(d.channel for d in defects if d))
    
    return result.solidify(DEFECTSET_VAL)

def make_defectdiff_iovs(defects1, defects2, system_only, ignore_not_considered):
    """
    Return a list of IoVs containing the set of defects only present in 
    defects1 or defects2
    """
    result = IOVSet()
    defectset = ((since, until, 
                  (t1.defects if t1 else set(), t2.defects if t2 else set()))
                 for since, until, (t1, t2) in process_iovs(defects1, defects2))
    for since, until, (t1defects, t2defects) in defectset:
        if t1defects == t2defects:
            continue
        
        diff = t1defects.symmetric_difference(t2defects)
        if ignore_not_considered and "GLOBAL_NOTCONSIDERED" in diff:
            continue

        tag1only, tag2only = t1defects & diff, t2defects & diff
                
        if system_only:
            tag1only = set(x.split("_")[0] for x in tag1only)
            tag2only = set(x.split("_")[0] for x in tag2only)
        
        result.add(since, until, tag1only, tag2only)
    
    return result.solidify(DEFECTDIFF_VAL)

def pretty(defects):
    return ", ".join(sorted(defects))

def main():
    import sys
    from argparse import ArgumentParser, FileType
    
    p = ArgumentParser(description="Compute differences in the defects "
                                   "database between tags")
    arg = p.add_argument
    arg("tags", metavar="DetStatus-Tag", nargs=2, type=str, 
        help="Two tags to show the difference between")
    arg("-p", "--project", default=None,
        help="Limit considered runs to those in project")
    arg("-P", "--period", default=None, help="Data period")
    arg("-r", "--run", type=int, help="Run number to process")
    arg("-R", "--range", help="Run range: e.g. 150000-151000")
    arg("-d", "--defect", action="append", dest="defects", default=None,
        help="Defects to show the difference between. "
             "(If empty, show all. -d can be specified multiple times.)")
    arg("-i", "--ignore-not-considered", action="store_true",
        help="Ignore ranges in which GLOBAL_NOTCONSIDERED is set for either tag")
    arg("--output", default=sys.stdout, type=FileType("w"), 
        help="Place to out the output (Default stdout)")
    arg("-s", "--by-system", action="store_true", 
        help="If specified, show systems which differ, not defects")
    arg("-v", "--virtual", action="store_true", 
        help="If specified, show virtual defect differences as well as primary"
             " (produces a lot of output)")
    
    args = p.parse_args()

    # tags
    tag1, tag2 = args.tags

    # range to process
    since, until = None, None
    #since, until = (200804, 0), (200804, 50)
    range_iovs = IOVSet()

    # if specifying run, only use that run, and it's easy
    if args.run:
        since, until = (args.run, 0), (args.run+1, 0)
        range_iovs = IOVSet.from_runs([args.run])
    elif args.range:
        run1, run2 = map(int, args.range.split("-"))
        since, until = (run1, 0), (run2, 0)
        range_iovs = IOVSet([ RANGEIOV_VAL(RunLumi(since), RunLumi(until)) ])

    # if specifying project, use that to build the range_iovs
    # TODO: what if period is specified but not project?
    #       shouldn't I define a default project in that case?
    elif args.project:

        # fetch the project:period:runs dictionary
        project_dict = fetch_project_period_runs()
        period_dict = project_dict[args.project]

        # if specifying period, use those runs
        runs = set()
        if args.period:
            runs = set(period_dict[args.period])
        else:
            #runs = set([ run for run in runs for (period, runs) in period_dict.iteritems() ])
            for period, period_runs in period_dict.iteritems():
                runs.update(period_runs)

        since, until = (min(runs), 0), (max(runs)+1, 0)
        range_iovs = IOVSet.from_runs(runs)

    # debugging
    print 'range to process:'
    print 'since, until =', since, until
    #print 'range_iovs:'
    #range_iovs.pprint()

    # arguments for fetching defects
    kwargs = dict(primary_only=not args.virtual, channels=args.defects, since=since, until=until)

    def fetch_defects(tag):
        iovs = make_defectset_iovs(range_iovs, DefectsDB(tag=tag).retrieve(**kwargs))
        return iovs

    print "Retrieving defects.."
    defects1, defects2 = fetch_defects(tag1), fetch_defects(tag2)
    #defects1.pprint()
    
    print "Computing diff.."
    diff_iovs = make_defectdiff_iovs(defects1, defects2, args.by_system, args.ignore_not_considered)
    for since, until, t1, t2 in diff_iovs:
        print since, until, "tag1:(%s)" % pretty(t1), "tag2:(%s)" % pretty(t2)

if __name__ == "__main__":
    raise SystemExit(main())
