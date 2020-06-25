#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

'''
Script for verifying menu configurations.

Can be run on individual files with the -hlt and/or -l1 options,
or on multiple files with the --folder option.
'''

import json
import os
import sys
import argparse

from collections import OrderedDict as odict
from AthenaCommon.Logging import logging
from menu_config_tests import TriggerLevel, menu_tests

log = logging.getLogger("TriggerMenuConfigTest")

def verify(config, trigger_level):
    '''
    Run all consistency checks for the trigger on the config.
    '''
    tests = menu_tests[trigger_level]
    for test in tests:
        test.run(config)

    successes = []
    failures = []
    for test in tests:
        if test.failures:
            failures.append(test)
        else:
            successes.append(test)
    return { "successes": successes, "failures": failures }


def load_and_verify(file_path, trigger_level):
    if not os.path.isfile(file_path):
        log.error("'{}' does not exist".format(file_path))
    elif not os.path.splitext(file_path)[1] == ".json":
        log.error("'{}' is not a JSON file".format(file_path))
    else:
        with open(file_path, "r") as config_file:
            config = json.load(config_file, object_pairs_hook = odict)

        return verify(config, trigger_level)

    return {"successes": [], "failures": []}


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Check a generated JSON configuration is valid")
    individual = parser.add_argument_group(
        title="Individual file options")
    individual.add_argument(
        "-hlt", help="Path to HLT menu config file")
    individual.add_argument(
        "-l1", help="Path to L1 menu config file")
    parser.add_argument("-f", "--folder", help="Path to a folder containing one or more configs to check")
    args = parser.parse_args()

    results = {}
    if args.folder:
        if not os.path.isdir(args.folder):
            log.error("Folder '{}' does not exist".format(args.folder))
            sys.exit(1)

        json_files = [f for f in os.listdir(args.folder)
                        if f.endswith("json")]
        def paths_starting_with(prefix):
            return [args.folder + os.sep + f
                    for f in json_files if f.startswith(prefix)]

        for hlt_menu in paths_starting_with("HLTMenu"):
            results[hlt_menu] = load_and_verify(
                hlt_menu, TriggerLevel.HLT)

        for l1_menu in paths_starting_with("L1Menu"):
            results[l1_menu] = load_and_verify(
                l1_menu, TriggerLevel.L1)
    else:
        if args.hlt:
            results[args.hlt] = load_and_verify(
                args.hlt, TriggerLevel.HLT)
        if args.l1:
            results[args.l1] = load_and_verify(
                args.l1, TriggerLevel.L1)

    any_failures = False
    for filename, result in results.items():
        success_plural = "" if len(result["successes"]) == 1 \
                         else "es"
        failure_plural = "" if len(result["failures"]) == 1 \
                         else "s"
        log.info(
            "{} success{}, {} failure{} for {}".format(
                len(result["successes"]), success_plural,
                len(result["failures"]), failure_plural,
                filename))
        for failed_test in result["failures"]:
            log.error("'{}' failed for: {}".format(
                failed_test.description,
                ", ".join([str(i) for i in failed_test.failures])))

        if result["failures"]:
            any_failures = True
    if any_failures:
        sys.exit(1)
