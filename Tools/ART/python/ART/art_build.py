#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import collections
import json
import multiprocessing
import re
import os

from art_misc import run_command, mkdir_p
from art_base import ArtBase
from art_header import ArtHeader

from parallelScheduler import ParallelScheduler


def run_job(art_directory, sequence_tag, script_directory, package, type, index, test_name):
    """TBD."""
    # print "run_job", art_directory, sequence_tag, script_directory, package, type, index, test_name
    (exit_code, out, err) = run_command(os.path.join(art_directory, './art-internal.py') + " job build " + script_directory + " " + package + " " + type + " " + sequence_tag + " " + str(index) + " " + "out")
    return (test_name, exit_code, out, err)


class ArtBuild(ArtBase):
    """TBD."""

    def __init__(self, art_directory, script_directory, max_jobs=0, ci=False):
        """TBD."""
        # print "ArtBuild", art_directory, script_directory, max_jobs
        self.art_directory = art_directory
        self.script_directory = script_directory
        self.max_jobs = multiprocessing.cpu_count() if max_jobs <= 0 else max_jobs
        self.ci = ci

    def task_list(self, type, sequence_tag):
        """TBD."""
        # print "task_list", type, sequence_tag
        test_directories = self.get_test_directories(self.script_directory)
        status = collections.defaultdict(lambda: collections.defaultdict(lambda: collections.defaultdict()))

        for package, root in test_directories.items():
            job_results = self.task(package, type, sequence_tag)
            for job_result in job_results:
                status[package][job_result[0]]['exit_code'] = job_result[1]
                status[package][job_result[0]]['out'] = job_result[2]
                status[package][job_result[0]]['err'] = job_result[3]

        with open(os.path.join(sequence_tag, "status.json"), 'w') as outfile:
            json.dump(status, outfile, sort_keys=True, indent=4, ensure_ascii=False)

        return 0

    def task(self, package, type, sequence_tag):
        """TBD."""
        # print "task", package, type, sequence_tag
        test_names = self.get_list(self.script_directory, package, type)
        scheduler = ParallelScheduler(self.max_jobs + 1)

        index = 0
        for test_name in test_names:
            schedule_test = False
            if self.ci:
                branch_name = os.environ['AtlasBuildBranch']
                fname = os.path.join(self.get_test_directories(self.script_directory)[package], test_name)
                cis = ArtHeader(fname).get('art-ci')
                for ci in cis:
                    if re.match(ci, branch_name):
                        schedule_test = True
                    break
            else:
                schedule_test = True

            if schedule_test:
                scheduler.add_task(task_name="t" + str(index), dependencies=[], description="d", target_function=run_job, function_kwargs={'art_directory': self.art_directory, 'sequence_tag': sequence_tag, 'script_directory': self.script_directory, 'package': package, 'type': type, 'index': index, 'test_name': test_name})
            index += 1

        return scheduler.run()

    def job(self, package, type, sequence_tag, index, out):
        """TBD."""
        # print "job", package, type, sequence_tag, index, out
        test_directories = self.get_test_directories(self.script_directory)
        test_directory = os.path.abspath(test_directories[package])
        test_name = self.get_files(test_directory, type)[int(index)]

        work_directory = os.path.join(sequence_tag, package, os.path.splitext(test_name)[0])
        mkdir_p(work_directory)

        (exit_code, output, err) = run_command(os.path.join(test_directory, test_name) + ' ' + '.' + ' ' + package + ' ' + type + ' ' + test_name, dir=work_directory)

        with open(os.path.join(work_directory, "stdout.txt"), "w") as text_file:
            text_file.write(output)
        with open(os.path.join(work_directory, "stderr.txt"), "w") as text_file:
            text_file.write(err)

        return exit_code
