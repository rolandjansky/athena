#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART-internal - ATLAS Release Tester (internal command).

Usage:
  art.py included            [-v]  <nightly_release> <project> <platform>
  art.py job local           [-v]  <script_directory> <package> <job_type> <sequence_tag> <index> <out>
  art.py job (grid|batch)    [-v]  <package> <job_type> <sequence_tag> <index> <out> <nightly_release> <project> <platform> <nightly_tag>
  art.py task local          [-v]  <script_directory> <package> <job_type> <sequence_tag>
  art.py task (grid|batch)   [-v]  <package> <job_type> <sequence_tag> <nightly_release> <project> <platform> <nightly_tag>
  art.py wait_for            [-v]  <nightly_release> <project> <platform> <nightly_tag>

Options:
  -h --help         Show this screen.
  --version         Show version.
  -v, --verbose     Show details.

Sub-commands:
  included          Check if a release and platform is included
  job               Runs a single job, given a particular index
  task              Runs a single task, consisting of given number of jobs
  wait_for          Wait for the release to be available

Arguments:
  index             Index of the test inside the package
  nightly_release   Name of the nightly release (e.g. 21.0)
  nightly_tag       Nightly tag (e.g. 2017-02-26T2119)
  out               Tar filename used for the output of the job
  package           Package of the test (e.g. Tier0ChainTests)
  platform          Platform (e.g. x86_64-slc6-gcc62-opt)
  project           Name of the project (e.g. Athena)
  script_directory  Directory containing the packages with tests
  sequence_tag      Sequence tag (e.g. 0 or PIPELINE_ID)
  job_type          Type of job (e.g. grid, ci, build)
"""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import os
import sys
from docopt_dispatch import dispatch

from art_local import ArtLocal
from art_grid import ArtGrid
from art_batch import ArtBatch


@dispatch.on('included')
def included(nightly_release, project, platform, **kwargs):
    """TBD."""
    sys.exit(ArtGrid(nightly_release, project, platform, None).included())


@dispatch.on('job', 'local')
def job_local(script_directory, package, job_type, sequence_tag, index, out, **kwargs):
    """TBD.

    Tests are called with the following parameters:
    SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME
    """
    print "job_local", script_directory, package, job_type, sequence_tag, index, out, kwargs
    ArtLocal(script_directory).job(package, job_type, sequence_tag, index, out)


@dispatch.on('job', 'grid')
def job_grid(package, job_type, sequence_tag, index, out, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD.

    Tests are called with the following parameters:
    SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME, NIGHTLY_RELEASE, PROJECT, PLATFORM, NIGHTLY_TAG
    """
    ArtGrid(nightly_release, project, platform, nightly_tag).job(package, job_type, sequence_tag, index, out)


@dispatch.on('job', 'batch')
def job_batch(package, job_type, sequence_tag, index, out, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD.

    Tests are called with the following parameters:
    SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME, NIGHTLY_RELEASE, PROJECT, PLATFORM, NIGHTLY_TAG
    """
    ArtBatch(nightly_release, project, platform, nightly_tag).job(package, job_type, sequence_tag, index, out)


@dispatch.on('task', 'local')
def task_local(script_directory, job_type, sequence_tag, **kwargs):
    """TBD."""
    ArtLocal(script_directory).task(job_type, sequence_tag)


@dispatch.on('task', 'grid')
def task_grid(package, job_type, sequence_tag, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    ArtGrid(nightly_release, project, platform, nightly_tag).task(package, job_type, sequence_tag)


@dispatch.on('task', 'batch')
def task_batch(package, job_type, sequence_tag, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    ArtBatch(nightly_release, project, platform, nightly_tag).task(package, sequence_tag)


@dispatch.on('wait_for')
def wait_for(nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    sys.exit(ArtGrid(nightly_release, project, platform, nightly_tag).wait_for())


if __name__ == '__main__':
    # NOTE: import should be here, to keep the order of the decorators (module first, art last and unused)
    from art import __version__
    dispatch(__doc__, version=os.path.splitext(os.path.basename(__file__))[0] + ' ' + __version__)
