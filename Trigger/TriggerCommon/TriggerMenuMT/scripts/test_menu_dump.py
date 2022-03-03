#!/usr/bin/env python3
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
Dumps the trigger menu, optionally running some checks for name
consistency.
"""
# help strings
_h_names = "print the names of all chains"
_h_parse = "parse names to dictionary"
_h_dev = "dump the dev menu (rather than physics)"

from argparse import ArgumentParser
import sys
from AthenaCommon.Logging import logging

# hack to turn off logging on imports
logging.INFO = logging.DEBUG

from TriggerMenuMT.HLT.Menu.Dev_pp_run3_v1 import setupMenu as setupDev
from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import setupMenu as setupPhys
from TriggerMenuMT.HLT.Config.Utility.DictFromChainName import dictFromChainName

def get_args():
    parser = ArgumentParser(description=__doc__)
    output = parser.add_mutually_exclusive_group()
    output.add_argument('-n', '--names', action='store_true',
                        help=_h_names)
    output.add_argument('-p', '--parse-names', action='store_true',
                        help=_h_parse)
    parser.add_argument('-d', '--dev-menu', action='store_true', help=_h_dev)
    return parser.parse_args()

def run():
    args = get_args()
    menu = setupDev() if args.dev_menu else setupPhys()
    if args.names:
        dump_chains(menu)
    elif args.parse_names:
        failed = find_parse_failures(menu)
        sys.exit(1 if failed else 0)

def chain_iter(menu):
    for group, chains in menu.items():
        for chain in chains:
            yield chain

def dump_chains(menu):
    try:
        for chain in chain_iter(menu):
            sys.stdout.write(f'{chain.name}\n')
    except BrokenPipeError:
        # this might happen if e.g. you are piping the output
        pass

def is_new_error(error):
    """
    Check for known issues.
    """
    # this happens because we don't set the input file in the
    # configuration flags. Not sure if there's an easy fix.
    if 'Input file name not set' in str(error):
        return False
    return True

def find_parse_failures(menu):
    """
    returns a set of failed chains
    """
    # disable even more useless output
    logging.WARNING = logging.DEBUG
    passed = set()
    known_failure = set()
    new_failure = set()
    for chain in chain_iter(menu):
        try:
            name = dictFromChainName(chain)['chainName']
            passed.add(name)
        except RuntimeError as err:
            if is_new_error(err):
                sys.stderr.write(f"can't parse {chain.name}: {err}\n")
                new_failure.add(chain.name)
            else:
                known_failure.add(chain.name)
    sys.stdout.write(
        f'Passed: {len(passed)}, Known failures: {len(known_failure)}\n')
    return new_failure

if __name__ == '__main__':
    run()
    sys.exit(0)
