#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import collections
import fnmatch
import json
import multiprocessing
import os
import re

from art_misc import run_command, mkdir_p
from art_base import ArtBase
from art_header import ArtHeader

from parallelScheduler import ParallelScheduler


def run_job(art_directory, sequence_tag, script_directory, package, job_type, index, test_name, nightly_release, project, platform, nightly_tag):
    """TBD."""
    print "job started", art_directory, sequence_tag, script_directory, package, job_type, index, test_name, nightly_release, project, platform, nightly_tag
    (exit_code, out, err) = run_command(' '.join((os.path.join(art_directory, './art-internal.py'), "job", "build", script_directory, package, job_type, sequence_tag, str(index), "out", nightly_release, project, platform, nightly_tag)))
    print "job ended", art_directory, sequence_tag, script_directory, package, job_type, index, test_name, nightly_release, project, platform, nightly_tag

    return (test_name, exit_code, out, err)


class ArtBuild(ArtBase):
    """TBD."""

    def __init__(self, art_directory, nightly_release, project, platform, nightly_tag, script_directory, max_jobs=0, ci=False):
        """TBD."""
        super(ArtBuild, self).__init__(art_directory)
        # print "ArtBuild", art_directory, script_directory, max_jobs
        self.art_directory = art_directory
        self.script_directory = script_directory.rstrip("/")
        self.nightly_release = nightly_release
        self.project = project
        self.platform = platform
        self.nightly_tag = nightly_tag
        self.max_jobs = multiprocessing.cpu_count() if max_jobs <= 0 else max_jobs
        self.ci = ci

    def task_list(self, job_type, sequence_tag):
        """TBD."""
        # print "task_list", job_type, sequence_tag
        test_directories = self.get_test_directories(self.script_directory)
        if not test_directories:
            print 'WARNING: No tests found in directories ending in "test"'

        status = collections.defaultdict(lambda: collections.defaultdict(lambda: collections.defaultdict()))

        for package, root in test_directories.items():
            test_directory = os.path.abspath(test_directories[package])
            job_results = self.task(package, job_type, sequence_tag)
            for job_result in job_results:
                test_name = job_result[0]
                status[package][test_name]['exit_code'] = job_result[1]
                status[package][test_name]['out'] = job_result[2]
                status[package][test_name]['err'] = job_result[3]
                status[package][test_name]['test_directory'] = test_directory

                # gather results
                result = []
                with open(os.path.join(sequence_tag, package, os.path.splitext(test_name)[0], 'stdout.txt'), 'r') as f:
                    for line in f:
                        match = re.search(r"art-result: (.*)", line)
                        if match:
                            result = json.loads(match.group(1))
                            break

                status[package][job_result[0]]['result'] = result

        mkdir_p(sequence_tag)
        with open(os.path.join(sequence_tag, "status.json"), 'w') as outfile:
            json.dump(status, outfile, sort_keys=True, indent=4, ensure_ascii=False)

        return 0

    def task(self, package, job_type, sequence_tag):
        """TBD."""
        # print "task", package, job_type, sequence_tag
        test_names = self.get_list(self.script_directory, package, job_type, "all")
        scheduler = ParallelScheduler(self.max_jobs + 1)

        index = 0
        for test_name in test_names:
            schedule_test = False
            fname = os.path.join(self.get_test_directories(self.script_directory)[package], test_name)
            if self.ci:
                branch_name = os.environ['AtlasBuildBranch']
                cis = ArtHeader(fname).get('art-ci')
                for ci in cis:
                    if fnmatch.fnmatch(branch_name, ci):
                        schedule_test = True
                    break
            else:
                schedule_test = True

            if not os.access(fname, os.X_OK):
                schedule_test = False
                print "job skipped, file not executable: ", fname

            if schedule_test:
                scheduler.add_task(task_name="t" + str(index), dependencies=[], description="d", target_function=run_job, function_kwargs={'art_directory': self.art_directory, 'sequence_tag': sequence_tag, 'script_directory': self.script_directory, 'package': package, 'job_type': job_type, 'index': index, 'test_name': test_name, 'nightly_release': self.nightly_release, 'project': self.project, 'platform': self.platform, 'nightly_tag': self.nightly_tag})
            index += 1

        result = scheduler.run()
        return result

    def job(self, package, job_type, sequence_tag, index, out):
        """TBD."""
        # print "job", package, job_type, sequence_tag, index, out
        test_directories = self.get_test_directories(self.script_directory)
        test_directory = os.path.abspath(test_directories[package])
        test_name = self.get_files(test_directory, job_type)[int(index)]

        work_directory = os.path.join(sequence_tag, package, os.path.splitext(test_name)[0])
        mkdir_p(work_directory)

        (exit_code, output, err) = run_command(' '.join((os.path.join(test_directory, test_name), '.', package, job_type, test_name, self.nightly_release, self.project, self.platform, self.nightly_tag)), dir=work_directory)

        with open(os.path.join(work_directory, "stdout.txt"), "w") as text_file:
            text_file.write(output)
        with open(os.path.join(work_directory, "stderr.txt"), "w") as text_file:
            text_file.write(err)

        return exit_code
