#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import os
import sys
import multiprocessing

from art_misc import run_command, verify, redirect, mkdir_p
from art_base import ArtBase

from parallelScheduler import ParallelScheduler


def run_job(sequence_tag, script_directory, package, type, index, test_name):
    """TBD."""
    print "run_job", sequence_tag, script_directory, package, type, index, test_name
    art_dir = os.path.dirname(os.path.realpath(sys.argv[0]))
    verify(run_command(os.path.join(art_dir, './art-internal.py') + " job local " + script_directory + " " + package + " " + type + " " + sequence_tag + " " + str(index) + " " + "out"))
    # print out


class ArtLocal(ArtBase):
    """TBD."""

    def __init__(self, script_directory, max_jobs=0):
        """TBD."""
        print "ArtLocal", script_directory, max_jobs
        self.script_directory = script_directory
        self.max_jobs = multiprocessing.cpu_count() if max_jobs <= 0 else max_jobs

    def task_list(self, type, sequence_tag):
        """TBD."""
        print "task_list", type, sequence_tag
        test_directories = self.get_test_directories(self.script_directory)
        for package, root in test_directories.items():
            self.task(package, type, sequence_tag)

    def task(self, package, type, sequence_tag):
        """TBD."""
        print "task", package, type, sequence_tag
        test_names = self.get_list(self.script_directory, package, type)
        scheduler = ParallelScheduler(self.max_jobs + 1)

        index = 0
        for test_name in test_names:
            scheduler.add_task(task_name="t" + str(index), dependencies=[], description="d", target_function=run_job, function_kwargs={'sequence_tag': sequence_tag, 'script_directory': self.script_directory, 'package': package, 'type': type, 'index': index, 'test_name': test_name})
            index += 1

        scheduler.run()

    def job(self, package, type, sequence_tag, index, out):
        """TBD."""
        print "job", package, type, sequence_tag, index, out
        test_directories = self.get_test_directories(self.script_directory)
        test_directory = os.path.abspath(test_directories[package])
        test_name = self.get_files(test_directory, type)[int(index)]

        work_directory = os.path.join(sequence_tag, package, os.path.splitext(test_name)[0])
        mkdir_p(work_directory)

        output = redirect(run_command(os.path.join(test_directory, test_name) + ' ' + '.' + ' ' + package + ' ' + type + ' ' + test_name, dir=work_directory, redirect=True))
        print output
