#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

import sys
import os
import re
import argparse
import subprocess
from TrigValTools.TrigValSteering import Common


def grep_errors(filename):
    '''Grep for error messages but ignore missing reference errors and echo commands'''
    error_pattern = '^.*ERROR.*$'
    ignore_patterns = [
        'Missing reference',
        'echo "ERROR'
    ]
    def filter_fn(msg):
        for p in ignore_patterns:
            if p in msg:
                return False
        return True

    lines = []
    with open(filename) as f:
        lines = re.findall(error_pattern, f.read(), re.MULTILINE)
    return list(filter(filter_fn, lines))


def main():
    log = Common.get_logger()

    parser = argparse.ArgumentParser(usage='%(prog)s PATH [PATH] ...')
    parser.add_argument('paths',
                        metavar='PATH',
                        nargs='+',
                        help='Path containing test python scripts to test')
    args = parser.parse_args()

    n_failed = 0
    for p in args.paths:
        if not os.path.exists(p):
            log.error('Path %s does not exist', p)
            return 1
        tests = [f for f in os.listdir(p) if f.startswith('test_') and f.endswith('.py')]
        log.info('Testing %d test scripts from path %s', len(tests), p)
        for test in tests:
            cmd = 'TRIGVALSTEERING_DRY_RUN=1 {:s}'.format(p+'/'+test)
            log_file = '{:s}.unitTest.log'.format(test)
            cmd += ' >{:s} 2>&1'.format(log_file)
            ret_code = subprocess.call(cmd, shell=True)
            failed = False
            status_str = 'OK'
            if ret_code != 0:
                failed = True
                status_str = 'FAILED WITH CODE {:d}'.format(ret_code)
            errors = grep_errors(log_file)
            if len(errors) > 0:
                failed = True
                status_str = 'ERROR IN LOG {:s}:'.format(log_file)
            log.info('---- %s ---- %s', test, status_str)

            if failed:
                n_failed += 1
                if len(errors) > 0:
                    for msg in errors:
                        print(msg)
                else:
                    log.error('Test failed but no ERROR messages found, printing full log below')
                    with open(log_file) as f:
                        for msg in f:
                            print(msg)

    return n_failed


if "__main__" in __name__:
    sys.exit(main())
