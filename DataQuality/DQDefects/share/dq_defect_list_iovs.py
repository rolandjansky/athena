#!/bin/env python
# Author: Steve Farrell <sfarrell@cern.ch>
# Part of the DataQuality/DQDefects package
# October 2012

import os
import sys
import fnmatch
import pickle
from collections import defaultdict
from datetime import datetime

from argparse import ArgumentParser
import logging; log = logging.getLogger('dq_defect_list_iovs')

from DQDefects import DefectsDB
from DQDefects.db import DEFAULT_CONNECTION_STRING

from DQUtils import IOVSet, process_iovs, define_iov_type
from DQUtils.sugar import RunLumi, RANGEIOV_VAL
from DQUtils.periods import fetch_project_period_runs


@define_iov_type
def DefectIOV(channel, comment):
    "Stores a defect IOV"

def get_project_dict():
    """
    Try to unpickle project dict from file first, 
    unless file doesn't exist or is older than 5 days
    """
    filename = 'project_period_runs.pickle'
    def get_file_age(name):
        delta = datetime.fromtimestamp(os.path.getmtime(name)) - datetime.now()
        return delta.days
    if os.path.exists(filename) and get_file_age(filename) < 5:
        print 'using existing pickled project dict:', filename
        with open(filename) as f:
            project_dict = pickle.load(f)
    else:
        # Make a new pickle file
        print 'storing project:period:runs dictionary to', filename
        with open(filename, 'w') as f:
            project_dict = fetch_project_period_runs()
            pickle.dump(project_dict, f)
    return project_dict


def main():
    parser = ArgumentParser(description='List defect IOVs')
    add_arg = parser.add_argument

    # Specify range to process
    add_arg('-p', '--project', default='data15_13TeV',
            help='Data project (default: data15_13TeV)')
    add_arg('-P', '--period', default=None, nargs='*', help='Data period(s)')
    #add_arg('-r', '--run', default=None, nargs='*', help='Run number(s) to process')
    add_arg('-r', '--run', default=None, type=int, help='Run number to process')
    add_arg('-R', '--range', help='Inclusive run range: e.g. 150000-151000')

    # Specify defects to process
    add_arg('-d', '--defects', default=None, nargs='*',
            help='Defects to process. Use * for wildcard (default: None)')

    # Other job options
    add_arg('-c', '--connection-string', default=DEFAULT_CONNECTION_STRING,
            help='Database connection to use (default: %s)' % DEFAULT_CONNECTION_STRING)
    #add_arg('-l', '--lumi-tag', default='OflLumi-8TeV-001',
            #help='Luminosity tag (default: OflLumi-8TeV-001)')
    add_arg('-t', '--tag', default='HEAD',
            help='Tag to use (default: HEAD)')

    # Parse arguments
    args = parser.parse_args()

    # Ranges to query
    since, until = None, None
    range_iovs = IOVSet()

    # If specifying runs, use those
    if args.run:
        since, until = (args.run, 0), (args.run+1, 0)
        range_iovs = IOVSet.from_runs([args.run])
    elif args.range:
        run1, run2 = map(int, args.range.split('-'))
        since, until = (run1, 0), (run2, 0)
        range_iovs = IOVSet([ RANGEIOV_VAL(RunLumi(since), RunLumi(until)) ])
    # Otherwise, use the project and period settings
    elif args.project:
        # Fetch the project:period:runs dictionary
        project_dict = get_project_dict()
        period_dict = project_dict[args.project]
        run_set = set()
        # Use periods if specified
        if args.period and len(args.period) > 0:
            for period in args.period:
                run_set.update(period_dict[period])
        # Otherwise use all periods in project
        else:
            for period, runs in period_dict.iteritems():
                run_set.update(runs)
        since, until = (min(run_set), 0), (max(run_set)+1, 0)
        range_iovs = IOVSet.from_runs(run_set)
    
    # debugging
    print 'range to process:'
    print 'since, until =', since, until

    # Instantiate the DB
    db = DefectsDB(args.connection_string, tag=args.tag)

    # Fetch the requested defect IOVs
    defect_iovs = db.retrieve(since, until, channels=args.defects)
    defect_iovset_list = defect_iovs.by_channel.values()

    # Get the overlap with the range_iovs
    # Use a dictionary of defect:iovset
    result_dict = defaultdict(IOVSet)
    for since, until, states in process_iovs(range_iovs, *defect_iovset_list):
        in_range, defects = states[0], states[1:]
        if in_range:
            for d in defects:
                if d: 
                    result_dict[d.channel].add(since, until, d.channel, d.comment)

    # Print the results
    for channel, result in result_dict.iteritems():
        result.solidify(DefectIOV)
        print '\n' + channel + '\n'
        result.pprint()


if __name__ == '__main__':
    main()
