#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

import sys
import os
import logging
import argparse
import re
import shutil
import subprocess
import json
from contextlib import contextmanager


@contextmanager
def remember_cwd():
    '''Simple pushd/popd replacement from https://stackoverflow.com/a/169112'''
    curdir = os.getcwd()
    try:
        yield
    finally:
        os.chdir(curdir)


def package_prefix(package):
    '''Returns a prefix included in names of all tests from the given package'''
    dict = {'TriggerTest':      'trig_',
            'TrigP1Test':       'trigP1_',
            'TrigAnalysisTest': 'trigAna_',
            'TrigUpgradeTest':  'trigUpgr_'}
    if package in dict:
        return dict[package]
    else:
        return None


def minimal_pattern(package):
    dict = {'TriggerTest':      None,
            'TrigP1Test':       None,
            'TrigAnalysisTest': None,
            'TrigUpgradeTest':  '(trigUpgr_full_menu_build|trigUpgr_newJO_build)'}
    if package in dict and dict[package] is not None:
        return dict[package]
    else:
        logging.error(
            "Minimal set of tests for %s is not defined.",
            "Please select specific tests using the name patter (option -n).")
        exit(1)

def duplicate_filename(list, filename):
    for path in list:
        if os.path.basename(path) == filename:
            return True
    return False

def find_scripts(patterns):
    scripts = []
    for path in os.environ['PATH'].split(':'):
        try:
            files = os.listdir(path)
        except:
            continue
        for filename in files:
            matched = True
            for patt in patterns:
                if re.search(patt, filename) is None:
                    matched = False
                    break
            if matched and not duplicate_filename(scripts,filename):
                scripts.append(path+'/'+filename)
    scripts.sort()
    return scripts


def get_parser():
    parser = argparse.ArgumentParser(usage='%(prog)s [options] PackageName')
    parser.add_argument('package',
                        metavar='PackageName',
                        help='Name of the package from which to run ART tests. Options are: %(choices)s',
                        choices=['TriggerTest', 'TrigAnalysisTest', 'TrigP1Test', 'TrigUpgradeTest'])
    parser.add_argument('-m', '--minimal',
                        action='store_true',
                        help='Run a small pre-defined set of tests for basic verification')
    parser.add_argument('-n', '--testName',
                        metavar='pattern',
                        help='Run only tests with a given pattern in the name')
    parser.add_argument('-t', '--artType',
                        metavar='type',
                        default='build',
                        choices=['build', 'grid'],
                        help='Run tests with the given art-type: build (default) or grid')
    parser.add_argument('-j', '--maxJobs',
                        metavar='N',
                        type=int,
                        default=1,
                        help='Run up to N tests in parallel (actual number may be limited by ART based on available CPUs and memory)')
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help='Increase output verbosity')
    return parser


def get_patterns(args):
    patterns = ['^test_', '(.sh|.py)$', package_prefix(args.package)]
    if args.testName:
        patterns.append(args.testName)
    if args.artType == 'grid':
        patterns.append('_grid.')
    else:
        patterns.append('_build.')
    if args.minimal:
        patterns.append(minimal_pattern(args.package))

    logging.debug("Searching for scripts with the following patterns: %s", patterns)
    return patterns


def analyse_results(all_test_results):
    '''Prints a summary table of all results and returns two lists. One includes names of failed tests,
    the other names of tests in which only the RootComp step failed. If only RootComp fails, the test is
    not added to the first list, as we currently do not enforce updating RootComp references on every change.'''
    failed_tests = []
    failed_rootcomp = []  # rootcomp failures are ignored at the moment
    table = {}  # test name : results
    for test_name in all_test_results.keys():
        results = all_test_results[test_name]['result']
        result_string = ""
        for step in results:
            result_string += "%s: %d, " % (step['name'], step['result'])
            if step['result'] != 0:
                if step['name'] == 'RootComp' and test_name not in failed_rootcomp:
                    failed_rootcomp.append(test_name)
                elif test_name not in failed_tests:
                    failed_tests.append(test_name)
        table[test_name] = result_string[:-2]
    max_len_col1 = len(max(table.keys(), key=len))
    max_len_col2 = len(max(table.values(), key=len))
    logging.info('-'*(max_len_col1+max_len_col2+7))
    for k, v in table.iteritems():
        logging.info('| {col1:<{width1}} | {col2:<{width2}} |'.format(
            col1=k, width1=max_len_col1,
            col2=v, width2=max_len_col2))
    logging.info('-'*(max_len_col1+max_len_col2+7))
    return failed_tests, failed_rootcomp


def print_summary(all_test_results, failed_tests, failed_rootcomp):
    if len(failed_rootcomp) > 0:
        logging.info(
            "RootComp failed in %d tests, but this step is currently ignored\n",
            len(failed_rootcomp))
    if len(failed_tests) > 0:
        logging.info(
            "%d tests succeeded out of %d executed",
            len(all_test_results)-len(failed_tests),
            len(all_test_results))
        logging.error("==================================================")
        logging.error("The following %d tests failed:", len(failed_tests))
        for test_name in failed_tests:
            logging.error("    %s", test_name)
        logging.error("==================================================")
    else:
        logging.info("==================================================")
        logging.info("All %d executed tests succeeded", len(all_test_results))
        logging.info("==================================================")

def prep_dirs(topdir, scripts):
    """ Creates test result structure if missing, if present clears the area only for the tests to be run"""
    import errno
    try:
        os.mkdir(topdir)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise e
    shutil.rmtree(topdir+'/test', ignore_errors=True)
    os.mkdir(topdir+'/test')

    # clear results dir
    for script in scripts:
        toerase=topdir+'/result/'+os.path.basename(script).replace('.sh', '')
        shutil.rmtree(toerase, ignore_errors=True)

def main():
    args = get_parser().parse_args()
    logging.basicConfig(stream=sys.stdout,
                        format='%(levelname)-8s %(message)s',
                        level=logging.DEBUG if args.verbose else logging.INFO)

    scripts = find_scripts(get_patterns(args))
    logging.info("The following %d tests will be executed: ", len(scripts))
    for filename in scripts:
        logging.info("    %s", os.path.basename(filename))

    topdir = 'runTrigART'
    success = True
    with remember_cwd():
        prep_dirs(topdir, scripts)
        os.chdir(topdir)
        for script_path in scripts:
            target = 'test/' + os.path.basename(script_path)
            os.symlink(script_path, target)

        # Run ART
        cmd = ["art.py", "run", "-q",
               "--max-jobs=%d" % args.maxJobs,
               "--type=%s" % args.artType,
               ".", "results"]
        s = " "
        logging.info("Executing ART command: %s", s.join(cmd))
        subprocess.call(cmd)
        logging.info("ART finished, analysing the results\n")

        # Read the result summary from JSON
        statusfile = 'results/status.json'
        if not os.path.isfile(statusfile):
            logging.error("ART status.json file is missing - likely the ART runner failed!")
            exit(1)
        with open(statusfile, 'r') as f:
            status_data = json.load(f)
            all_test_results = status_data['.']
            if len(all_test_results) != len(scripts):
                logging.warning("Selected %d tests but ART executed only %d. Please check why some tests did not run!")
            failed_tests, failed_rootcomp = analyse_results(all_test_results)
            print_summary(all_test_results, failed_tests, failed_rootcomp)
            if len(failed_tests) > 0:
                success = False

    if not success:
        exit(1)
    else:
        exit(0)


if "__main__" in __name__:
    sys.exit(main())
