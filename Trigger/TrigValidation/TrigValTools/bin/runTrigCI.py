#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''Script to run several Trigger ART test scripts and make a summary for Continuous Integration framework'''

import os
import sys
import logging
import argparse
import subprocess
import errno
from collections import OrderedDict
from TrigValTools.TrigARTUtils import find_scripts, remember_cwd


def get_parser():
    parser = argparse.ArgumentParser(usage='%(prog)s [options] scripts',
                                     description=__doc__)
    parser.add_argument('scripts',
                    metavar='scripts',
                    nargs='+',
                    help='Scripts to run (can be multiple)')
    parser.add_argument('-r', '--required-step',
                        metavar='STEP_NAME',
                        action='append',
                        help='Add step name to explicitly check in addition to the exit code.'
                             ' This option can be specified multiple times')
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help='Increase output verbosity')
    return parser


def analyse_steps(log_file, steps):
    art_results = {}
    if not os.path.isfile(log_file):
        logging.error('CANNOT OPEN LOG FILE %s', log_file)
        return 1

    with open(log_file) as f:
        for line in f:
            if not line.startswith('art-result:'):
                continue
            logging.debug('PARSING RESULT LINE: %s', line)
            split_line = line.split(' ')
            if len(split_line) < 3:
                logging.warning('CANNOT PARSE STEP RESULT FROM LINE: %s', line)
                continue
            art_results[split_line[2].strip()] = int(split_line[1].strip())

    logging.debug('FOUND THE FOLLOWING RESULTS: %s', art_results.keys())
    ret_code = 0
    for step_name in steps:
        if step_name not in art_results.keys():
            logging.error('NO RESULT FOUND FOR REQUIRED STEP %s', step_name)
            return 1
        result = art_results[step_name]
        if result == 0:
            logging.debug('STEP %s SUCCEEDED WITH CODE %d', step_name, result)
        else:
            logging.error('STEP %s FAILED WITH CODE %d', step_name, result)
        if abs(result) > ret_code:
            ret_code = abs(result)

    return ret_code


def try_mkdir(dirname):
    try:
        os.mkdir(dirname)
    except OSError as e:
        if e.errno == errno.EEXIST:
            logging.info("SUB-DIRECTORY %s ALREADY EXISTS AND WILL BE REUSED", dirname)
        else:
            logging.error("FAILED TO CREATE SUB-DIRECTORY %s", dirname)
            raise e


def main():
    args = get_parser().parse_args()
    logging.basicConfig(stream=sys.stdout,
                        format='========== %(levelname)-8s %(message)s',
                        level=logging.DEBUG if args.verbose else logging.INFO)

    results = OrderedDict()
    max_name_len = 0
    for script in args.scripts:
        logging.debug('PROCESSING SCRIPT %s', script)
        found_scripts = find_scripts([script])
        if len(found_scripts)==0 or os.path.basename(found_scripts[0]) != script:
            logging.error('THE SCRIPT %s IS NOT INSTALLED, CANNOT RUN THE TEST', script)
            results[script] = 1
        if len(script) > max_name_len:
            max_name_len = len(script)
        with remember_cwd():
            dirname = os.path.splitext(script)[0]
            try_mkdir(dirname)
            os.chdir(dirname)
            logging.info('='*50)
            logging.info('STARTING %s IN SUB-DIRECTORY %s', script, dirname)
            logging.info('='*50)
            cmd = '{:s} 2>&1 | tee stdout.txt; exit ${{PIPESTATUS[0]}}'.format(script)
            logging.debug('EXECUTING COMMAND IN A SUBSHELL: %s', cmd)
            ret_code = subprocess.call(cmd, shell=True)
            if not args.required_step:
                logging.info('FINISHED %s WITH EXIT CODE %d', script, ret_code)
            else:
                logging.info('FINISHED %s WITH EXIT CODE %d, CHECKING RESULTS FOR EXTRA REQUIRED STEPS: %s',
                             script, ret_code, ' '.join(args.required_step))
                ret_code = analyse_steps('stdout.txt', args.required_step)
                logging.info('POST-PROCESSING RESULT OF %s IS CODE %d\n', script, ret_code)
            results[script] = ret_code

    logging.info('RESULTS SUMMARY:')
    logging.info('='*(max_name_len+11))
    final_code = 0
    for script, result in results.items():
        logging.info('| %s : %4d |', '{:{width}s}'.format(script, width=max_name_len), result)
        if abs(result) > final_code:
            final_code = abs(result)
    logging.info('| %s : %4d |', '{:{width}s}'.format('FINAL CODE', width=max_name_len), final_code)
    logging.info('='*(max_name_len+11))

    return final_code


if "__main__" in __name__:
    sys.exit(main())
