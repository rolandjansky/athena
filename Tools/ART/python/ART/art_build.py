#!/usr/bin/env python
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
"""Class for (local) build submits."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import collections
import concurrent.futures
import fnmatch
import json
import logging
import multiprocessing
import os
import socket

from art_misc import memory, mkdir, run_command, GByte
from art_base import ArtBase
from art_header import ArtHeader

MODULE = "art.build"


def run_job(art_directory, sequence_tag, script_directory, package, job_type, job_index, test_name):
    """
    Job to be run by parallel or serial scheduler.

    Needs to be defined outside a class.
    Names of arguments are important, see call to scheduler.
    """
    # <script_directory> <sequence_tag> <package> <outfile> <job_type> <job_index>
    log = logging.getLogger(MODULE)
    log.info("job started %s %s %s %s %s %d %s", art_directory, sequence_tag, script_directory, package, job_type, job_index, test_name)
    (exit_code, out, err, command, start_time, end_time) = run_command(' '.join((os.path.join(art_directory, './art-internal.py'), "build", "job", script_directory, sequence_tag, package, "out", job_type, str(job_index))))
    log.info("job ended %s %s %s %s %s %d %s", art_directory, sequence_tag, script_directory, package, job_type, job_index, test_name)

    return (package, test_name, exit_code, out, err, start_time, end_time)


class ArtBuild(ArtBase):
    """Class for (local) build submits."""

    def __init__(self, art_directory, nightly_release, project, platform, nightly_tag, script_directory, max_jobs=0, ci=False):
        """Keep arguments."""
        super(ArtBuild, self).__init__(art_directory)
        log = logging.getLogger(MODULE)
        log.debug("ArtBuild %s %s %d", art_directory, script_directory, max_jobs)
        self.art_directory = art_directory
        self.script_directory = script_directory.rstrip("/")
        self.nightly_release = nightly_release
        self.project = project
        self.platform = platform
        self.nightly_tag = nightly_tag
        mem = memory(GByte)
        max_cores = min(mem / 4, multiprocessing.cpu_count())
        max_cores = max_cores if max_cores >= 4 else 1
        self.max_jobs = max_cores if max_jobs <= 0 else max_jobs
        self.ci = ci

    def task_list(self, job_type, sequence_tag):
        """Run a list of packages for given job_type with sequence_tag."""
        log = logging.getLogger(MODULE)
        log.debug("task_list %s %s", job_type, sequence_tag)
        test_directories = self.get_test_directories(self.script_directory)
        if not test_directories:
            log.warning('No tests found in directories ending in "test"')

        log.info("Executor started with %d threads", self.max_jobs)
        executor = concurrent.futures.ThreadPoolExecutor(max_workers=self.max_jobs)
        future_set = []

        for package, directory in test_directories.items():
            future_set.extend(self.task(executor, package, job_type, sequence_tag))

        # Create status of all packages
        status = collections.defaultdict(lambda: collections.defaultdict(lambda: collections.defaultdict()))

        # Some release information
        status['release_info']['nightly_release'] = self.nightly_release
        status['release_info']['nightly_tag'] = self.nightly_tag
        status['release_info']['project'] = self.project
        status['release_info']['platform'] = self.platform
        status['release_info']['hostname'] = socket.gethostname()

        # Package information with all tests in each package
        for future in concurrent.futures.as_completed(future_set):
            (package, test_name, exit_code, out, err, start_time, end_time) = future.result()
            log.debug("Handling job for %s %s", package, test_name)
            status[package][test_name]['exit_code'] = exit_code
            status[package][test_name]['start_time'] = start_time.strftime('%Y-%m-%dT%H:%M:%S')
            status[package][test_name]['end_time'] = end_time.strftime('%Y-%m-%dT%H:%M:%S')
            status[package][test_name]['start_epoch'] = start_time.strftime('%s')
            status[package][test_name]['end_epoch'] = end_time.strftime('%s')

            test_directory = os.path.abspath(test_directories[package])
            fname = os.path.join(test_directory, test_name)
            if os.path.exists(fname):
                status[package][test_name]['description'] = ArtHeader(fname).get(ArtHeader.ART_DESCRIPTION)
            else:
                log.warning("Test file cannot be opened to get description: %s", fname)
                status[package][test_name]['description'] = ""
            status[package][test_name]['test_directory'] = test_directory

            # gather results
            result = []
            stdout_path = os.path.join(sequence_tag, package, os.path.splitext(test_name)[0], 'stdout.txt')
            log.debug("Looking for results in %s", stdout_path)
            if os.path.exists(stdout_path):
                with open(stdout_path, 'r') as f:
                    output = f.read()
                    result = ArtBase.get_art_results(output)
            else:
                log.warning("Output file does not exist: %s", stdout_path)

            status[package][test_name]['result'] = result

        mkdir(sequence_tag)
        with open(os.path.join(sequence_tag, "status.json"), 'w') as outfile:
            json.dump(status, outfile, sort_keys=True, indent=4, ensure_ascii=False)

        return 0

    def task(self, executor, package, job_type, sequence_tag):
        """Run tests of a single package."""
        log = logging.getLogger(MODULE)
        log.debug("task %s %s %s", package, job_type, sequence_tag)
        test_directories = self.get_test_directories(self.script_directory)
        test_directory = os.path.abspath(test_directories[package])
        test_names = self.get_files(test_directory, job_type, "all", self.nightly_release, self.project, self.platform)
        if not test_names:
            log.debug("No tests found for package %s and job_type %s", package, job_type)

        future_set = []
        job_index = 0
        for test_name in test_names:
            schedule_test = False
            fname = os.path.join(test_directory, test_name)
            if self.ci:
                branch_name = os.environ['AtlasBuildBranch']
                cis = ArtHeader(fname).get(ArtHeader.ART_CI)
                for ci in cis:
                    if fnmatch.fnmatch(branch_name, ci):
                        schedule_test = True
                    break
            else:
                schedule_test = True

            if not os.access(fname, os.X_OK):
                schedule_test = False
                log.warning("job skipped, file not executable: %s", fname)

            if schedule_test:
                future_set.append(executor.submit(run_job, self.art_directory, sequence_tag, self.script_directory, package, job_type, job_index, test_name))
            job_index += 1

        return future_set

    def job(self, sequence_tag, package, out, job_type, job_index):
        """Run a single test."""
        log = logging.getLogger(MODULE)
        log.debug("ArtBuild job %s %s %s %d %s", package, job_type, sequence_tag, job_index, out)
        test_directories = self.get_test_directories(self.script_directory)
        test_directory = os.path.abspath(test_directories[package])
        test_name = self.get_files(test_directory, job_type, "all", self.nightly_release, self.project, self.platform)[int(job_index)]

        work_directory = os.path.join(sequence_tag, package, os.path.splitext(test_name)[0])
        mkdir(work_directory)
        log.debug("Work dir %s", work_directory)

        # Tests are called with arguments: PACKAGE TEST_NAME SCRIPT_DIRECTORY TYPE
        script_directory = '.'
        env = os.environ.copy()
        env['ArtScriptDirectory'] = script_directory
        env['ArtPackage'] = package
        env['ArtJobType'] = job_type
        env['ArtJobName'] = test_name
        cmd = ' '.join((os.path.join(test_directory, test_name), package, test_name, script_directory, job_type))
        (exit_code, output, err, command, start_time, end_time) = run_command(cmd, dir=work_directory, env=env)

        with open(os.path.join(work_directory, "stdout.txt"), "w") as text_file:
            log.debug("Copying stdout into %s", work_directory)
            text_file.write(output)
        with open(os.path.join(work_directory, "stderr.txt"), "w") as text_file:
            log.debug("Copying stderr into %s", work_directory)
            text_file.write(err)

        return exit_code
