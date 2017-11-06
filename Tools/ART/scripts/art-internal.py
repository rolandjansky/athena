#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART-internal - ATLAS Release Tester (internal command).

Usage:
  art-internal.py job build   [-v]  <script_directory> <package> <job_type> <sequence_tag> <index> <out> <nightly_release> <project> <platform> <nightly_tag>
  art-internal.py job grid    [-v --skip-setup]  <script_directory> <package> <job_type> <sequence_tag> <index> <out> <nightly_release> <project> <platform> <nightly_tag>
  art-internal.py task build  [-v]  <script_directory> <package> <job_type> <sequence_tag> <nightly_release> <project> <platform> <nightly_tag>
  art-internal.py task grid   [-v --skip-setup]  <submit_directory> <script_directory> <package> <job_type> <sequence_tag> <nightly_release> <project> <platform> <nightly_tag>

Options:
  --skip-setup      Do not run atlas setup or voms
  -h --help         Show this screen.
  -v, --verbose     Show details.
  --version         Show version.

Sub-commands:
  job               Runs a single job, given a particular index
  task              Runs a single task, consisting of given number of jobs

Arguments:
  index             Index of the test inside the package
  nightly_release   Name of the nightly release (e.g. 21.0)
  nightly_tag       Nightly tag (e.g. 2017-02-26T2119)
  out               Tar filename used for the output of the job
  package           Package of the test (e.g. Tier0ChainTests)
  platform          Platform (e.g. x86_64-slc6-gcc62-opt)
  project           Name of the project (e.g. Athena)
  script_directory  Directory containing the package(s) with tests
  sequence_tag      Sequence tag (e.g. 0 or PIPELINE_ID)
  submit_directory  Temporary directory with all files for submission
  job_type          Type of job (e.g. grid, ci, build)
"""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import os
import sys

from ART.docopt_dispatch import dispatch

from ART import ArtGrid, ArtBuild


@dispatch.on('job', 'build')
def job_build(script_directory, package, job_type, sequence_tag, index, out, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD.

    Tests are called with the following parameters:
    SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME
    """
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    exit(ArtBuild(art_directory, nightly_release, project, platform, nightly_tag, script_directory).job(package, job_type, sequence_tag, index, out))


@dispatch.on('job', 'grid')
def job_grid(script_directory, package, job_type, sequence_tag, index, out, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD.

    Tests are called with the following parameters:
    SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME, NIGHTLY_RELEASE, PROJECT, PLATFORM, NIGHTLY_TAG
    """
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    skip_setup = kwargs['skip_setup']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag, script_directory, skip_setup).job(package, job_type, sequence_tag, index, out))


@dispatch.on('task', 'build')
def task_build(script_directory, job_type, sequence_tag, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    exit(ArtBuild(art_directory, nightly_release, project, platform, nightly_tag, script_directory).task(job_type, sequence_tag))


@dispatch.on('task', 'grid')
def task_grid(submit_directory, script_directory, package, job_type, sequence_tag, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    skip_setup = kwargs['skip_setup']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag, script_directory, skip_setup, submit_directory).task(package, job_type, sequence_tag))


if __name__ == '__main__':
    # NOTE: import should be here, to keep the order of the decorators (module first, art last and unused)
    from art import __version__
    print "ART_PATH", os.path.dirname(os.path.realpath(sys.argv[0]))
    dispatch(__doc__, version=os.path.splitext(os.path.basename(__file__))[0] + ' ' + __version__)
