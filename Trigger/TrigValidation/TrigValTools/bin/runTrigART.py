#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

import sys
import os
import logging
import argparse
import shutil
import subprocess
import json
from TrigValTools.TrigARTUtils import package_prefix, find_scripts, remember_cwd


def minimal_pattern(package):
    dict = {'TriggerTest':      '(test_trig_data_v1Dev_build|test_trig_data_newJO_build)',
            'TrigP1Test':       'test_trigP1_v1Dev_build',
            'TrigAnalysisTest': 'test_trigAna_RDOtoRDOTrig_v1Dev_build'}
    if package == 'ALL':
        return '({})'.format('|'.join([v for v in dict.values() if v]))
    elif package in dict and dict[package] is not None:
        return dict[package]
    else:
        logging.error("Minimal set of tests for %s is not defined.", package)
        exit(1)


def get_parser():
    packages = ['TriggerTest', 'TrigAnalysisTest', 'TrigP1Test', 'ALL']
    parser = argparse.ArgumentParser(usage='%(prog)s [options] [PackageName]')
    parser.add_argument('package',
                        metavar='PackageName',
                        default='ALL',
                        nargs='?',
                        help='Name of the package from which to run ART tests. Options are: %(choices)s.'
                             ' If no name is provided, %(default)s is used.',
                        choices=packages)
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
    parser.add_argument('-d', '--dryRun',
                        action='store_true',
                        help='List tests which would be executed, but don\'t execute them')

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


def analyse_result_python(test_results):
    '''Analyses test results based on the final exit code, which determines
    the status for ART tests based on TrigValSteering python framework'''
    result_string = ''
    for step in test_results['result']:
        result_string += "%s: %d, " % (step['name'], step['result'])
    result_string += "exit: %d" % test_results['exit_code']
    is_success = (test_results['exit_code'] == 0)
    return result_string, is_success


def analyse_result_shell(test_results):
    '''Analyses test results based on individual hard-coded step names, as
    the final status cannot be unambiguously determined in the old shell-based
    ART tests not using the TrigValSteering framework'''
    result_string = ''
    steps_to_ignore = ['RootComp', 'MessageCount']
    is_success = True
    for step in test_results['result']:
        result_string += "%s: %d, " % (step['name'], step['result'])
        if step['name'] not in steps_to_ignore and step['result'] != 0:
            is_success = False
    result_string += "exit: %d" % test_results['exit_code']
    return result_string, is_success


def analyse_results(all_test_results):
    '''Prints a summary table of all results and returns two lists. One includes names of failed tests,
    the other names of tests in which only the RootComp step failed. If only RootComp fails, the test is
    not added to the first list, as we currently do not enforce updating RootComp references on every change.'''
    failed_tests = []
    table = {}  # test name : results
    for test_name in all_test_results.keys():
        if test_name.endswith('.py'):
            result_string, is_success = analyse_result_python(all_test_results[test_name])
        else:
            result_string, is_success = analyse_result_shell(all_test_results[test_name])
        table[test_name] = result_string
        if not is_success:
            failed_tests.append(test_name)
    max_len_col1 = len(max(table.keys(), key=len))
    max_len_col2 = len(max(table.values(), key=len))
    logging.info('-'*(max_len_col1+max_len_col2+7))
    for k, v in table.items():
        logging.info('| {col1:<{width1}} | {col2:<{width2}} |'.format(
            col1=k, width1=max_len_col1,
            col2=v, width2=max_len_col2))
    logging.info('-'*(max_len_col1+max_len_col2+7))
    return failed_tests


def print_summary(all_test_results, failed_tests):
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
        toerase = topdir+'/result/'+os.path.basename(script).replace('.sh', '')
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

    if len(scripts) > 5*args.maxJobs:
        if args.maxJobs == 1:
            logging.warning("You are running %d tests in sequence. This may take "
                            "a long time, consider using -j N option.", len(scripts))
        else:
            logging.warning("You are running %d tests with %d parallel jobs. "
                            "This may take a long time.", len(scripts), args.maxJobs)

    if args.dryRun:
        return 0

    topdir = 'runTrigART'
    success = True
    with remember_cwd():
        prep_dirs(topdir, scripts)
        os.chdir(topdir)
        for script_path in scripts:
            target = 'test/' + os.path.basename(script_path)
            os.symlink(script_path, target)

        # Set up and run ART
        commands = [
            'export ATLAS_LOCAL_ROOT_BASE="${ATLAS_LOCAL_ROOT_BASE:-/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase}"',
            'source "${ATLAS_LOCAL_ROOT_BASE}"/user/atlasLocalSetup.sh --quiet',
            'lsetup -q art']
        art_cmd = 'art.py run --run-all-tests --max-jobs={:d} {:s} . results'.format(args.maxJobs, '-v' if args.verbose else '-q')
        commands.append(art_cmd)
        cmd = ' && '.join(commands)
        logging.info("Executing ART command: %s", art_cmd)
        subprocess.call(cmd, shell=True)
        logging.info("ART finished, analysing the results\n")

        # Read the result summary from JSON
        statusfile = 'results/{:s}-status.json'.format(topdir)
        if not os.path.isfile(statusfile):
            logging.error("ART status.json file is missing - likely the ART runner failed!")
            exit(1)
        with open(statusfile, 'r') as f:
            status_data = json.load(f)
            all_test_results = status_data[topdir]
            if len(all_test_results) != len(scripts):
                logging.warning("Selected %d tests but ART executed only %d. Please check why some tests did not run!")
            failed_tests = analyse_results(all_test_results)
            print_summary(all_test_results, failed_tests)
            if len(failed_tests) > 0:
                success = False

    if not success:
        exit(1)
    else:
        exit(0)


if "__main__" in __name__:
    sys.exit(main())
