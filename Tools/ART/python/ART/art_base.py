#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import fnmatch
import inspect
import os
import sys
import yaml

from art_misc import run_command
from art_header import ArtHeader


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

    def list(self, package, type, json_format=False):
        """TBD."""
        self.not_implemented()

    def log(self, package, test_name):
        """TBD."""
        self.not_implemented()

    def output(self, package, test_name, file_name):
        """TBD."""
        self.not_implemented()

    def validate(self, script_directory):
        """TBD."""
        directories = self.get_test_directories(script_directory.rstrip("/"))
        for directory in directories.itervalues():
            files = self.get_files(directory)
            for fname in files:
                ArtHeader(os.path.join(directory, fname)).validate()
        return 0

    #
    # Default implementations
    #
    def compare_ref(self, file_name, ref_file, entries=-1):
        """TBD."""
        import PyUtils.PoolFile as PF

        # diff-pool
        df = PF.DiffFiles(refFileName=ref_file, chkFileName=file_name, ignoreList=['RecoTimingObj_p1_RAWtoESD_timings', 'RecoTimingObj_p1_ESDtoAOD_timings'])
        df.printSummary()
        stat = df.status()
        print stat
        del df

        # diff-root
        (code, out, err) = run_command("acmd.py diff-root " + file_name + " " + ref_file + " --error-mode resilient --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings RecoTimingObj_p1_RAWtoESD_mems RecoTimingObj_p1_RAWtoESD_timings RAWtoESD_mems RAWtoESD_timings ESDtoAOD_mems ESDtoAOD_timings HITStoRDO_timings RAWtoALL_mems RAWtoALL_timings RecoTimingObj_p1_RAWtoALL_mems RecoTimingObj_p1_RAWtoALL_timings RecoTimingObj_p1_EVNTtoHITS_timings --entries " + str(entries))
        if code != 0:
            print "Error:", code
            print "StdErr:", err

        print out
        sys.stdout.flush()
        return err

    #
    # Protected Methods
    #
    def get_config(self):
        """Retrieve dictionary of ART configuration file."""
        config_file = open("art-configuration.yml", "r")
        config = yaml.load(config_file)
        config_file.close()
        return config

    def get_files(self, directory, type=None):
        """
        Return a list of all test files matching 'test_*.sh' of given 'type'.

        If type is None, all files are returned. Only the filenames are returned.
        """
        result = []
        if directory is not None:
            files = os.listdir(directory)
            files.sort()
            for fname in files:
                if fnmatch.fnmatch(fname, 'test_*.sh') or fnmatch.fnmatch(fname, 'test_*.py'):
                    if type is None or ArtHeader(os.path.join(directory, fname)).get('art-type') == type:
                        result.append(fname)
        return result

    def get_type(self, directory, test_name):
        """Return the 'type' of a test."""
        return ArtHeader(os.path.join(directory, test_name)).get('art-type')

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
