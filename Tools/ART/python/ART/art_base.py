#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Base class for grid and (local) build submits."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import fnmatch
import inspect
import json
import logging
import os
import re

try:
    import scandir as scan
except ImportError:
    import os as scan

from art_configuration import ArtConfiguration
# from art_diff import ArtDiff
from art_header import ArtHeader
from art_misc import is_exe, run_command

MODULE = "art.base"


class ArtBase(object):
    """Base class for grid and (local) build submits."""

    def __init__(self, art_directory):
        """Keep arguments."""
        self.art_directory = art_directory

    def task_list(self, job_type, sequence_tag):
        """Default implementation."""
        self.not_implemented()

    def task(self, package, job_type, sequence_tag):
        """Default implementation."""
        self.not_implemented()

    def job(self, package, job_type, sequence_tag, index, out):
        """Default implementation."""
        self.not_implemented()

    def compare(self, package, test_name, days, file_names):
        """Default implementation."""
        self.not_implemented()

    def list(self, package, job_type, json_format=False):
        """Default implementation."""
        self.not_implemented()

    def log(self, package, test_name):
        """Default implementation."""
        self.not_implemented()

    def output(self, package, test_name, file_name):
        """Default implementation."""
        self.not_implemented()

    def validate(self, script_directory):
        """Validate all tests in given script_directory."""
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
        """Print all included tests for these arguments."""
        log = logging.getLogger(MODULE)
        directories = self.get_test_directories(script_directory.rstrip("/"))
        for directory in directories.itervalues():
            files = self.get_files(directory, job_type, index_type)
            for fname in files:
                test_name = os.path.join(directory, fname)
                if self.is_included(test_name, nightly_release, project, platform):
                    log.info("%s %s", test_name, ArtHeader(test_name).get(ArtHeader.ART_INCLUDE))
        return 0

    def config(self, package, nightly_release, project, platform, config):
        """Show configuration."""
        log = logging.getLogger(MODULE)
        config = ArtConfiguration(config)
        keys = config.keys(nightly_release, project, platform, package)
        for key in keys:
            log.info("%s %s", key, config.get(nightly_release, project, platform, package, key))
        return 0

    def download(self, input_file):
        """Download input_file from RUCIO."""
        return self.get_input(input_file)

    #
    # Default implementations
    #
    def compare_ref(self, path, ref_path, entries=-1):
        """TBD."""
        result = 0

        (exit_code, out, err) = run_command(' '.join(("art-diff.py", "--diff-type=diff-pool", path, ref_path)))
        if exit_code != 0:
            result |= exit_code
            print err
        print out

        (exit_code, out, err) = run_command(' '.join(("art-diff.py", "--diff-type=diff-root", "--entries=" + str(entries), path, ref_path)))
        if exit_code != 0:
            result |= exit_code
            print err
        print out

        return result

    #
    # Protected Methods
    #
    @staticmethod
    def get_art_results(output):
        """
        Extract art-results.

        find all
        'art-result: x' or 'art-result: x name' or 'art-result: [x]'
        and append them to result list
        """
        result = []
        for line in output.splitlines():
            match = re.search(r"art-result: (\d+)\s*(.*)", line)
            if match:
                item = json.loads(match.group(1))
                name = match.group(2)
                result.append({'name': name, 'result': item})
            else:
                match = re.search(r"art-result: (\[.*\])", line)
                if match:
                    array = json.loads(match.group(1))
                    for item in array:
                        result.append({'name': '', 'result': item})

        return result

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
                if job_type is not None and ArtHeader(test_name).get(ArtHeader.ART_TYPE) != job_type:
                    continue

                # is not included in nightly_release, project, platform
                if nightly_release is not None and not self.is_included(test_name, nightly_release, project, platform):
                    continue

                # batch and does specify art-input
                if index_type == "batch" and ArtHeader(test_name).get(ArtHeader.ART_INPUT) is not None:
                    continue

                # single and does not specify art-input
                if index_type == "single" and ArtHeader(test_name).get(ArtHeader.ART_INPUT) is None:
                    continue

                result.append(fname)

        return result

    def get_type(self, directory, test_name):
        """Return the 'job_type' of a test."""
        return ArtHeader(os.path.join(directory, test_name)).get(ArtHeader.ART_TYPE)

    def get_test_directories(self, directory):
        """
        Search from '<directory>...' for '<package>/test' directories.

        A dictionary key=<package>, value=<directory> is returned
        """
        result = {}
        for root, dirs, files in scan.walk(directory):
            # exclude some directories
            dirs[:] = [d for d in dirs if not d.endswith('_test.dir')]
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
        patterns = ArtHeader(test_name).get(ArtHeader.ART_INCLUDE)

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
        """Default Not Implemented Method."""
        raise NotImplementedError("Class %s doesn't implement method: %s(...)" % (self.__class__.__name__, inspect.stack()[1][3]))
