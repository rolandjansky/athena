#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""
__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import fnmatch
import inspect
import os
import re
import sys
import yaml

from art_misc import check, run_command


class ArtBase(object):
    """TBD."""

    def __init__(self):
        """TBD."""
        pass

    def task_list(self, type, sequence_tag):
        """TBD."""
        self.not_implemented()

    def task(self, package, type, sequence_tag):
        """TBD."""
        self.not_implemented()

    def job(self, package, type, sequence_tag, index, out):
        """TBD."""
        self.not_implemented()

    def compare(self, package, test_name, days, file_names):
        """TBD."""
        self.not_implemented()

    def list(self, package, type):
        """TBD."""
        self.not_implemented()

    def log(self, package, test_name):
        """TBD."""
        self.not_implemented()

    def output(self, package, test_name, file_name):
        """TBD."""
        self.not_implemented()

    def included(self):
        """TBD."""
        self.not_implemented()

    def wait_for(self):
        """TBD."""
        self.not_implemented()

    #
    # Default implementations
    #
    def compare_ref(self, file_name, ref_file, entries=-1):
        """TBD."""
        out = check(run_command("acmd.py diff-root " + file_name + " " + ref_file + " --error-mode resilient --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings RecoTimingObj_p1_RAWtoESD_mems RecoTimingObj_p1_RAWtoESD_timings RAWtoESD_mems RAWtoESD_timings ESDtoAOD_mems ESDtoAOD_timings HITStoRDO_timings RAWtoALL_mems RAWtoALL_timings RecoTimingObj_p1_RAWtoALL_mems RecoTimingObj_p1_RAWtoALL_timings --entries " + str(entries)))
        print out
        sys.stdout.flush()

    #
    # Protected Methods
    #
    def get_config(self):
        """Retrieve dictionary of ART configuration file."""
        config_file = open("art-configuration.yml", "r")
        config = yaml.load(config_file)
        config_file.close()
        return config

    def get_art_headers(self, filename):
        """Return dictionary with art headers."""
        result = {}
        for line in open(filename, "r"):
            line_match = re.match(r'#\s*art-(\w+):\s+(.+)$', line)
            if line_match:
                result[line_match.group(1)] = line_match.group(2)
        return result

    def get_files(self, directory, type):
        """Return a list of all test files matching 'test_*.sh' of given 'queue'."""
        result = []
        if directory is not None:
            files = os.listdir(directory)
            files.sort()
            for fname in files:
                if fnmatch.fnmatch(fname, 'test_*.sh') or fnmatch.fnmatch(fname, 'test_*.py'):
                    headers = self.get_art_headers(os.path.join(directory, fname))
                    if 'type' in headers and headers['type'] == type:
                        result.append(fname)
        return result

    def get_type(self, directory, test_name):
        """Return the 'type' of a test."""
        headers = self.get_art_headers(os.path.join(directory, test_name))
        return None if 'type' not in headers else headers['type']

    def get_test_directories(self, directory):
        """
        Search from '<directory>...' for '<package>/test' directories.

        A dictionary key=<package>, value=<directory> is returned
        """
        result = {}
        for root, dirs, files in os.walk(directory):
            if root.endswith('/test'):
                package = os.path.basename(os.path.dirname(root))
                result[package] = root
        return result

    def get_list(self, directory, package, type):
        """Return a list of tests for a particular package."""
        test_directories = self.get_test_directories(directory)
        test_dir = test_directories[package]
        return self.get_files(test_dir, type)

    #
    # Private Methods
    #
    def not_implemented(self):
        """TBD."""
        raise NotImplementedError("Class %s doesn't implement method: %s(...)" % (self.__class__.__name__, inspect.stack()[1][3]))
