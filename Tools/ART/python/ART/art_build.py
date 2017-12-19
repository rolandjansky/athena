#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Class for (local) build submits."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import collections
import fnmatch
import json
import logging
import multiprocessing
import os

from art_misc import run_command, mkdir_p
from art_base import ArtBase
from art_header import ArtHeader

from parallelScheduler import ParallelScheduler

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
    (exit_code, out, err) = run_command(' '.join((os.path.join(art_directory, './art-internal.py'), "build", "job", script_directory, sequence_tag, package, "out", job_type, str(job_index))))
    log.info("job ended %s %s %s %s %s %d %s", art_directory, sequence_tag, script_directory, package, job_type, job_index, test_name)

    return (test_name, exit_code, out, err)


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
        self.max_jobs = multiprocessing.cpu_count() if max_jobs <= 0 else max_jobs
        self.ci = ci

    def task_list(self, job_type, sequence_tag):
        """Run a list of packages for given job_type with sequence_tag."""
        log = logging.getLogger(MODULE)
        log.debug("task_list %s %s", job_type, sequence_tag)
        test_directories = self.get_test_directories(self.script_directory)
        if not test_directories:
            log.warning('No tests found in directories ending in "test"')

        status = collections.defaultdict(lambda: collections.defaultdict(lambda: collections.defaultdict()))

        for package, directory in test_directories.items():
            test_directory = os.path.abspath(test_directories[package])
            job_results = self.task(package, job_type, sequence_tag)
            for job_result in job_results:
                test_name = job_result[0]
                status[package][test_name]['exit_code'] = job_result[1]
                # Removed, seem to give empty lines
                # status[package][test_name]['out'] = job_result[2]
                # status[package][test_name]['err'] = job_result[3]
                fname = os.path.join(test_directory, test_name)
                status[package][test_name]['description'] = ArtHeader(fname).get(ArtHeader.ART_DESCRIPTION)
                status[package][test_name]['test_directory'] = test_directory

                # gather results
                result = []
                log.debug("Looking for results for test %s", test_name)
                with open(os.path.join(sequence_tag, package, os.path.splitext(test_name)[0], 'stdout.txt'), 'r') as f:
                    output = f.read()
                    result = ArtBase.get_art_results(output)

                status[package][test_name]['result'] = result

        mkdir_p(sequence_tag)
        with open(os.path.join(sequence_tag, "status.json"), 'w') as outfile:
            json.dump(status, outfile, sort_keys=True, indent=4, ensure_ascii=False)

        return 0

    def task(self, package, job_type, sequence_tag):
        """Run tests of a single package."""
        log = logging.getLogger(MODULE)
        log.debug("task %s %s %s", package, job_type, sequence_tag)
        test_directories = self.get_test_directories(self.script_directory)
        test_directory = os.path.abspath(test_directories[package])
        test_names = self.get_files(test_directory, job_type, "all", self.nightly_release, self.project, self.platform)
        scheduler = ParallelScheduler(self.max_jobs + 1)

        index = 0
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
                scheduler.add_task(task_name="t" + str(index), dependencies=[], description="d", target_function=run_job, function_kwargs={'art_directory': self.art_directory, 'sequence_tag': sequence_tag, 'script_directory': self.script_directory, 'package': package, 'job_type': job_type, 'job_index': index, 'test_name': test_name})
            index += 1

        result = scheduler.run()
        return result

    def job(self, sequence_tag, package, out, job_type, job_index):
        """Run a single test."""
        log = logging.getLogger(MODULE)
        log.debug("ArtBuild job %s %s %s %d %s", package, job_type, sequence_tag, job_index, out)
        test_directories = self.get_test_directories(self.script_directory)
        test_directory = os.path.abspath(test_directories[package])
        test_name = self.get_files(test_directory, job_type, "all", self.nightly_release, self.project, self.platform)[int(job_index)]

        work_directory = os.path.join(sequence_tag, package, os.path.splitext(test_name)[0])
        mkdir_p(work_directory)

        # Tests are called with arguments: PACKAGE TEST_NAME SCRIPT_DIRECTORY TYPE
        script_directory = '.'
        env = os.environ.copy()
        env['ArtScriptDirectory'] = script_directory
        env['ArtPackage'] = package
        env['ArtJobType'] = job_type
        env['ArtJobName'] = test_name
        cmd = ' '.join((os.path.join(test_directory, test_name), package, test_name, script_directory, job_type))
        (exit_code, output, err) = run_command(cmd, dir=work_directory, env=env)

        with open(os.path.join(work_directory, "stdout.txt"), "w") as text_file:
            text_file.write(output)
        with open(os.path.join(work_directory, "stderr.txt"), "w") as text_file:
            text_file.write(err)

        return exit_code
