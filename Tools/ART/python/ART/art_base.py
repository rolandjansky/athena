#!/usr/bin/env python
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
"""Base class for grid and (local) build submits."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import calendar
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

from datetime import datetime

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
        log.info("Validating script directory: %s", script_directory)
        directories = self.get_test_directories(script_directory.rstrip("/"))

        found_test = False
        valid = True
        for directory in directories.itervalues():
            files = self.get_files(directory, valid_only=False)
            for fname in files:
                test_name = os.path.join(directory, fname)
                found_test = True
                log.debug(test_name)
                if not is_exe(test_name):
                    valid &= False
                    log.error("%s is not executable.", test_name)
                valid &= ArtHeader(test_name).validate()

        if not found_test:
            log.warning('No scripts found in %s directory', directories.values()[0])
            return 1

        if not valid:
            log.error('Some scripts have invalid headers or are not executable')
            return 1

        log.info("Scripts in %s directory are valid", script_directory)
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
        if package is None:
            log.info("%s", config.packages())
            return 0

        keys = config.keys(nightly_release, project, platform, ArtConfiguration.ALL)
        keys.update(config.keys(nightly_release, project, platform, package))
        for key in keys:
            log.info("'%s'-'%s'", key, config.get(nightly_release, project, platform, package, key))
        return 0

    #
    # Default implementations
    #
    def compare_ref(self, path, ref_path, files, diff_pool, diff_root, entries=-1, mode='detailed'):
        """TBD."""
        result = 0

        if diff_pool:
            (exit_code, out, err, command, start_time, end_time) = run_command(' '.join(("art-diff.py", "--diff-type=diff-pool", path, ref_path)))
            if exit_code != 0:
                result |= exit_code
                print err
            print out

        if diff_root:
            (exit_code, out, err, command, start_time, end_time) = run_command(' '.join(("art-diff.py", "--diff-type=diff-root", "--mode=" + mode, "--entries=" + str(entries), (' '.join(('--file=' + s for s in files))), path, ref_path)))
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

    def run_on(self, on_days):
        """Return True if this is a good day to runself."""
        log = logging.getLogger(MODULE)
        if on_days is None or len(on_days) == 0:
            return True

        today = datetime.today()

        for on_day in on_days:
            # Check for Monday, Tuesday, ...
            if on_day.lower() == calendar.day_name[today.weekday()].lower():
                return True

            # weekends
            if on_day.lower() == "weekends" and calendar.day_name[today.weekday()] in ['Saturday', 'Sunday']:
                return True

            # weekdays
            if on_day.lower() == "weekdays" and calendar.day_name[today.weekday()] not in ['Saturday', 'Sunday']:
                return True

            # odd days
            if on_day.lower() == "odd days" and today.day % 2 != 0:
                return True

            # even days
            if on_day.lower() == "even days" and today.day % 2 == 0:
                return True

            # list of day numbers
            days = on_day.split(',')
            try:
                for day in days:
                    if int(day) == today.day:
                        return True
            except Exception:
                log.warning("Some item in art_runon list '%s' is not an integer or keyword, skipped", on_days)
                return False

        return False

    def get_files(self, directory, job_type=None, index_type="all", nightly_release=None, project=None, platform=None, run_all_tests=False, valid_only=True):
        """
        Return a list of all test files matching 'test_*.sh' of given 'job_type', 'index_type' and nightly/project/platform.

        'job_type' can be 'grid' or 'build', given by the test

        'index_type' can be 'all', 'batch' or 'single'.

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
                header = ArtHeader(test_name)
                has_art_input = header.get(ArtHeader.ART_INPUT) is not None
                has_art_athena_mt = header.get(ArtHeader.ART_ATHENA_MT) > 0

                # SKIP if file is not valid
                if valid_only and not header.is_valid():
                    continue

                # SKIP if art-type not defined
                if header.get(ArtHeader.ART_TYPE) is None:
                    continue

                # SKIP if is not of correct type
                if job_type is not None and header.get(ArtHeader.ART_TYPE) != job_type:
                    continue

                # SKIP if is not included in nightly_release, project, platform
                if not run_all_tests and nightly_release is not None and not self.is_included(test_name, nightly_release, project, platform):
                    continue

                # SKIP if batch and does specify art-input or art-athena-mt
                if index_type == "batch" and (has_art_input or has_art_athena_mt):
                    continue

                # SKIP if single and does NOT specify art-input or art-athena-mt
                if index_type == "single" and not (has_art_input or has_art_athena_mt):
                    continue

                # SKIP if this is not a good day to run
                if not self.run_on(header.get(ArtHeader.ART_RUNON)):
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
            platform_pattern = "*-*-*-*"

            count = pattern.count('/')
            if count >= 2:
                (nightly_release_pattern, project_pattern, platform_pattern) = pattern.split('/', 3)
            elif count == 1:
                (nightly_release_pattern, project_pattern) = pattern.split('/', 2)
            elif pattern != "":
                nightly_release_pattern = pattern

            if fnmatch.fnmatch(nightly_release, nightly_release_pattern) and fnmatch.fnmatch(project, project_pattern) and fnmatch.fnmatch(platform, platform_pattern):
                return True
        return False

    #
    # Private Methods
    #
    def not_implemented(self):
        """Default Not Implemented Method."""
        raise NotImplementedError("Class %s doesn't implement method: %s(...)" % (self.__class__.__name__, inspect.stack()[1][3]))
