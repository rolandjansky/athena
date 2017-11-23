#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import fnmatch
import inspect
import logging
import os
import yaml

try:
    import scandir as scan
except ImportError:
    import os as scan

from art_misc import is_exe, run_command
from art_header import ArtHeader

MODULE = "art.base"


class ArtBase(object):
    """TBD."""

    def __init__(self, art_directory):
        """TBD."""
        self.art_directory = art_directory

    def task_list(self, job_type, sequence_tag):
        """TBD."""
        self.not_implemented()

    def task(self, package, job_type, sequence_tag):
        """TBD."""
        self.not_implemented()

    def job(self, package, job_type, sequence_tag, index, out):
        """TBD."""
        self.not_implemented()

    def compare(self, package, test_name, days, file_names):
        """TBD."""
        self.not_implemented()

    def list(self, package, job_type, json_format=False):
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
        log = logging.getLogger(MODULE)
        directories = self.get_test_directories(script_directory.rstrip("/"))

        found_test = False
        for directory in directories.itervalues():
            files = self.get_files(directory)
            for fname in files:
                test_name = os.path.join(directory, fname)
                found_test = True
                log.debug(test_name)
                if not is_exe(test_name):
                    log.error("%s is not executable.", test_name)
                ArtHeader(test_name).validate()

        if not found_test:
            log.warning('No scripts found in %s directory', directories.values()[0])
            return 0

        log.info("Scripts in %s directory are validated", script_directory)
        return 0

    def included(self, script_directory, job_type, index_type, nightly_release, project, platform):
        """TBD."""
        log = logging.getLogger(MODULE)
        directories = self.get_test_directories(script_directory.rstrip("/"))
        for directory in directories.itervalues():
            files = self.get_files(directory, job_type, index_type)
            for fname in files:
                test_name = os.path.join(directory, fname)
                if self.is_included(test_name, nightly_release, project, platform):
                    log.info("%s %s", test_name, ArtHeader(test_name).get('art-include'))
        return 0

    def download(self, input_file):
        """TBD."""
        return self.get_input(input_file)

    #
    # Default implementations
    #
    def compare_ref(self, file_name, ref_file, entries=-1):
        """TBD."""
        import PyUtils.PoolFile as PF

        log = logging.getLogger(MODULE)

        # diff-pool
        df = PF.DiffFiles(refFileName=ref_file, chkFileName=file_name, ignoreList=['RecoTimingObj_p1_RAWtoESD_timings', 'RecoTimingObj_p1_ESDtoAOD_timings'])
        df.printSummary()
        stat = df.status()
        print stat
        del df

        # diff-root
        (code, out, err) = run_command("acmd.py diff-root " + file_name + " " + ref_file + " --error-mode resilient --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings RecoTimingObj_p1_RAWtoESD_mems RecoTimingObj_p1_RAWtoESD_timings RAWtoESD_mems RAWtoESD_timings ESDtoAOD_mems ESDtoAOD_timings HITStoRDO_timings RAWtoALL_mems RAWtoALL_timings RecoTimingObj_p1_RAWtoALL_mems RecoTimingObj_p1_RAWtoALL_timings RecoTimingObj_p1_EVNTtoHITS_timings --entries " + str(entries))
        if code != 0:
            log.error("Error: %d", code)
            print(err)

        log.info(out)
        return code

    #
    # Protected Methods
    #
    def get_config(self):
        """Retrieve dictionary of ART configuration file, or None if file does not exist."""
        try:
            config_file = open("art-configuration.yml", "r")
            config = yaml.load(config_file)
            config_file.close()
            return config
        except IOError:
            return None

    def get_files(self, directory, job_type=None, index_type="all", nightly_release=None, project=None, platform=None):
        """
        Return a list of all test files matching 'test_*.sh' of given 'job_type', 'index_type' and nightly/project/platform.

        'index_type' can be 'all', 'batch' or 'single'.

        If "given" is None, all files are returned.

        Only the filenames are returned.
        """
        result = []
        if directory is not None:
            files = os.listdir(directory)
            files.sort()
            for fname in files:
                # is not a test ?
                if not fnmatch.fnmatch(fname, 'test_*.sh') and not fnmatch.fnmatch(fname, 'test_*.py'):
                    continue

                test_name = os.path.join(directory, fname)

                # is not of correct type
                if job_type is not None and ArtHeader(test_name).get('art-type') != job_type:
                    continue

                # is not included in nightly_release, project, platform
                if nightly_release is not None and not self.is_included(test_name, nightly_release, project, platform):
                    continue

                # batch and does specify art-input
                if index_type == "batch" and ArtHeader(test_name).get('art-input') is not None:
                    continue

                # single and does not specify art-input
                if index_type == "single" and ArtHeader(test_name).get('art-input') is None:
                    continue

                result.append(fname)

        return result

    def get_type(self, directory, test_name):
        """Return the 'job_type' of a test."""
        return ArtHeader(os.path.join(directory, test_name)).get('art-type')

    def get_test_directories(self, directory):
        """
        Search from '<directory>...' for '<package>/test' directories.

        A dictionary key=<package>, value=<directory> is returned
        """
        result = {}
        for root, dirs, files in scan.walk(directory):
            if root.endswith('/test'):
                package = os.path.basename(os.path.dirname(root))
                result[package] = root
        return result

    def get_list(self, directory, package, job_type, index_type):
        """Return a list of tests for a particular package."""
        test_directories = self.get_test_directories(directory)
        test_dir = test_directories[package]
        return self.get_files(test_dir, job_type, index_type)

    def is_included(self, test_name, nightly_release, project, platform):
        """Return true if a match is found for test_name in nightly_release, project, platform."""
        patterns = ArtHeader(test_name).get('art-include')

        for pattern in patterns:
            nightly_release_pattern = "*"
            project_pattern = "*"
            platform_pattern = "*-*-*-opt"

            count = pattern.count('/')
            if count >= 2:
                (nightly_release_pattern, project_pattern, platform_pattern) = pattern.split('/', 3)
            elif count == 1:
                (nightly_release_pattern, project_pattern) = pattern.split('/', 2)
            else:
                nightly_release_pattern = pattern

            if fnmatch.fnmatch(nightly_release, nightly_release_pattern) and fnmatch.fnmatch(project, project_pattern) and fnmatch.fnmatch(platform, platform_pattern):
                return True
        return False

    def get_input(self, input_name):
        """Download input file from rucio. Retuns path of inputfile."""
        work_dir = '.'

        # run in correct environment
        env = os.environ.copy()
        env['PATH'] = '.:' + env['PATH']

        (code, out, err) = run_command(os.path.join(self.art_directory, "art-get-input.sh") + " " + input_name, dir=work_dir, env=env)
        if code == 0 and out != '':
            return os.path.join(work_dir, input_name.replace(':', '/', 1))

        return None

    #
    # Private Methods
    #
    def not_implemented(self):
        """TBD."""
        raise NotImplementedError("Class %s doesn't implement method: %s(...)" % (self.__class__.__name__, inspect.stack()[1][3]))
