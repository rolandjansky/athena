#!/bin/env python
# Author: Steve Farrell <sfarrell@cern.ch>
# Part of the DataQuality/DQDefects package
# June 2012

import sys
import fnmatch

from argparse import ArgumentParser
from textwrap import dedent
import logging; log = logging.getLogger("dq_defect_summary")

from DQDefects import DefectsDB
from DQDefects.db import DEFAULT_CONNECTION_STRING

from DQUtils.general import timer
from DQUtils.logger import init_logger
from DQUtils.lumi import fetch_lumi_inputs, compute_lumi, compute_lumi_many_channels
from DQUtils.periods import fetch_project_period_runs


class Defect(object):
    """
    Class to hold all the information needed for printing DQ summary for one defect
    """
    def __init__(self, name, description, depends, virtual, intolerable, lumi):
        if description == name:
            # Don't bother pretending that there is a description if it is just
            # equal to the name
            description = ""

        (self.name, self.description, self.depends, self.lumi, self.virtual,
         self.intolerable) = (
         name, description, depends, lumi, virtual, intolerable)

        self.lumiStr = "{0:<.2f}".format(self.lumi)


def print_virtual_tree(defect, all_defects, depth=0, max_depth=0):

    if defect.lumi > 0 or depth == 0:
        print "|     "*depth + defect.lumiStr, defect.name

        if max_depth < 0 or depth < max_depth:
            parents = [ all_defects[d] for d in defect.depends ]

            # Sort list of parents by lumi
            parents.sort(key=lambda d: d.lumi, reverse=True)
            
            # Recursively call this function on defects down the tree
            for parent in parents:
                print_virtual_tree(parent, all_defects, depth+1, max_depth)

    
def get_primary_defects(defect, all_defects):
    primary_defects = set()
    def recursive_search(defect):
        if defect.virtual:
            for parent in defect.depends:
                recursive_search(all_defects[parent])
        else:
            primary_defects.add(defect)
    recursive_search(defect)
    return primary_defects

def print_primary_defects(defect, all_defects, max_primary):
    primary_list = sorted(get_primary_defects(defect, all_defects),
                          key=lambda d: d.lumi, reverse=True)
    print defect.lumiStr, defect.name
    if max_primary >= 0:
        del primary_list[max_primary:]
    for primary in primary_list:
        if primary.lumi > 0:
            print "      " + primary.lumiStr, primary.name

def main():
    parser = ArgumentParser(description="Summarize DQ Defects")
    a = parser.add_argument
    add_group = parser.add_argument_group

    # Specify range to process
    a("-p", "--project", default="data15_13TeV",
      help="Data project (default: data15_13TeV)")
    a("-P", "--period", default=None, nargs="*", help="Data period(s)")
    a("-r", "--run", default=None, nargs="*", help="Run number(s) to process")
    a("-R", "--range", help="Inclusive run range: e.g. 150000-151000")

    # Specify defects to process
    a("-d", "--defects", default=None, nargs="*",
      help="Defects to process. Use * for wildcard (default: None)")

    # Print mode, primary or virtual tree
    a("-q", "--tree", action="store_true",
      help="Dump virtual defect tree. Set depth with -D")

    # Other job options
    a("-n", "--num-primary", default=-1, type=int,
      help="Max number of primary defects to display in default mode (default: all)")
    a("-D", "--depth", default=-1, type=int,
      help="Max virtual defect depth to print in virtual tree mode (default: all)")
    a("-c", "--connection-string", default=DEFAULT_CONNECTION_STRING,
      help="Database connection to use (default: %s)" % DEFAULT_CONNECTION_STRING)
    a("-l", "--lumi-tag", default='OflLumi-8TeV-002', 
      help="Luminosity tag (default: OflLumi-8TeV-002)")
    a("-t", "--tag", default="HEAD",
      help="Tag to use (default: HEAD)")
    a("--require-ready", default=True, type=int,
      help="Calculate luminosity with respect to ATLAS READY (default: True)")
    a("--reject-busy", default=False, type=int,
      help="Calculate luminosity with respect to not-busy (default: False)")
    a("--nb", action="store_true", help="Show lumi in units of 1/nb instead of 1/pb")
    a("-v", "--verbose", default=1, type=int,
      help="Set verbosity (default: 1)")

    args = parser.parse_args()

    init_logger(verbose=args.verbose)

    # Units
    units = 1e3 if args.nb else 1e6
    unit_string = "(1/nb)" if args.nb else "(1/pb)"

    # Instantiate the database interface
    with timer("Instantiate DefectsDB"):
        db = DefectsDB(args.connection_string, tag=args.tag)

    # Set runs to process
    # TODO: control these via run lumi iovs, rather than a set of good_runs
    #       similar to how it is done in dq_defect_compare_tags
    good_runs = None
    if args.range:
        since, until = map(int, args.range.split("-"))
    elif args.run:
        good_runs = set( map(int, args.run) )
        #since, until = args.run, args.run+1
        since, until = min(good_runs), max(good_runs)+1
    else:
        project_dict = fetch_project_period_runs()
        if args.period:
            #good_runs = set( project_dict[args.project][args.period] )
            good_runs = set()
            for period in args.period:
                good_runs.update( project_dict[args.project][period] )
            since, until = min(good_runs), max(good_runs)+1
        else:
            good_runs = set()
            for period, period_runs in project_dict[args.project].iteritems():
                good_runs.update(period_runs)
            since, until = min(good_runs), max(good_runs)+1


    iov_range = (since, 0), (until, 0)
    log.info("Processing range: " + str(iov_range))
    #log.info(good_runs)

    # Fetch all defect IOVs in range
    with timer("Fetch defects"):
        all_defect_iovs = db.retrieve(*iov_range)

    # Grab defect information
    with timer("Fetch defect info"):
        descriptions = db.all_defect_descriptions
        intolerables = db.get_intolerable_defects()
        virtuals = db.virtual_defect_names

    # Grab lbs and lumi variables in range
    with timer("Fetch lumi inputs"):
        lbs, lumis = fetch_lumi_inputs(iov_range, args.lumi_tag)

    # Compute lumi per channel
    with timer("Compute luminosities"):
        # Filtering defects
        exclude_iovs = []
        # Defect for ATLAS READY
        if args.require_ready:
            exclude_iovs.append(all_defect_iovs.by_channel["GLOBAL_NOTREADY"])
        # Defect for detector busy
        if args.reject_busy:
            exclude_iovs.append(all_defect_iovs.by_channel["GLOBAL_BUSY"])
        # Compute total luminosity
        lumi_total = compute_lumi(lbs, lumis, lbs, exclude_iovsets=exclude_iovs, 
                                  good_runs=good_runs)/units
        # Compute luminosity for all defects
        lumi_by_defect = compute_lumi_many_channels(lbs, lumis, all_defect_iovs, 
                                                    exclude_iovsets=exclude_iovs,
                                                    good_runs=good_runs)

    my_defect_names = []
    if args.defects:
        all_defect_names = db.defect_names | db.virtual_defect_names
        for defect_arg in args.defects:
            my_defect_names.extend(sorted(fnmatch.filter(all_defect_names, defect_arg)))

    logics = db.virtual_defect_logics

    # Build the defect objects
    all_defects = {}
    for name, description in sorted(descriptions.iteritems()):
        virtual, intolerable = name in virtuals, name in intolerables
        depends = []
        logic = logics.get(name, None)
        if logic: depends = logic.clauses
        all_defects[name] = Defect(name, description, depends, virtual, intolerable,
                                   lumi_by_defect.get(name, 0)/units)

    # My defects
    my_defects = [ all_defects[d] for d in my_defect_names ]
    my_defects.sort(key=lambda d: d.lumi, reverse=True)

    print "\nTotal luminosity", unit_string, "\n"
    print "{0:<.2f}".format(lumi_total)

    print "\nDefect luminosity", unit_string, "\n"

    #for defect_name in my_defect_names:
    for defect in my_defects:

        if args.tree:
            # Virtual defect tree
            print_virtual_tree(defect, all_defects, depth=0, max_depth=args.depth)
        else:
            # Primary defect dump
            print_primary_defects(defect, all_defects, max_primary=args.num_primary)

    print ""


if __name__ == "__main__":
    main()
