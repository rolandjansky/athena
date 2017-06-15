#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART - ATLAS Release Tester.

Usage:
  art.py run             [-v --type=<T> --max-jobs=<N>]  <script_directory> <sequence_tag>
  art.py submit          [-v --type=<T>]  <sequence_tag> <nightly_release> <project> <platform> <nightly_tag>
  art.py compare grid    [-v --days=<D>]  <nightly_release> <project> <platform> <nightly_tag> <package> <test_name> <file_name>...
  art.py compare ref     [-v]  <file_name> <ref_file>
  art.py list grid       [-v]  <package> <job_type> <nightly_release> <project> <platform> <nightly_tag>
  art.py log grid        [-v]  <package> <test_name> <nightly_release> <project> <platform> <nightly_tag>
  art.py output grid     [-v]  <package> <test_name> <file_name> <nightly_release> <project> <platform> <nightly_tag>

Options:
  -h --help         Show this screen.
  --version         Show version.
  -v, --verbose     Show details.
  --type=<T>        Type of job (e.g. grid, ci, build)
  --days=<D>        Number of days ago to pick up reference for compare [default: 1]
  --max-jobs=<N>    Maximum number of concurrent jobs to run

Sub-commands:
  run               Run tests from a package locally
  submit            Submit tests to the grid
  compare           Compare the output of a job
  list              Lists the jobs of a package
  log               Show the log of a job
  output            Get the output of a job

Arguments:
  file_name         Filename to save the output to
  index             Index of the test inside the package
  nightly_release   Name of the nightly release (e.g. 21.0)
  nightly_tag       Nightly tag (e.g. 2017-02-26T2119)
  out               Tar filename used for the output of the job
  package           Package of the test (e.g. Tier0ChainTests)
  platform          Platform (e.g. x86_64-slc6-gcc62-opt)
  project           Name of the project (e.g. Athena)
  script_directory  Directory containing the packages with tests
  sequence_tag      Sequence tag (e.g. 0 or PIPELINE_ID)
  test_name         Name of the test inside the package (e.g. test_q322.sh)
  job_type          Type of job (e.g. grid, ci, build)
"""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"
__version__ = '0.0.3'

import re
import requests
import os
import sys


from docopt_dispatch import dispatch
from art_base import ArtBase
from art_local import ArtLocal
from art_grid import ArtGrid


@dispatch.on('submit')
def submit_grid(sequence_tag, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    type = 'grid' if kwargs['type'] is None else kwargs['type']
    ArtGrid(nightly_release, project, platform, nightly_tag).task_list(type, sequence_tag)


@dispatch.on('run')
def run(script_directory, sequence_tag, **kwargs):
    """TBD."""
    type = 'build' if kwargs['type'] is None else kwargs['type']
    ArtLocal(script_directory, max_jobs=kwargs['max_jobs']).task_list(type, sequence_tag)


@dispatch.on('compare', 'ref')
def compare_local(file_name, ref_file, **kwargs):
    """TBD."""
    ArtBase().compare_ref(file_name, ref_file)


@dispatch.on('compare', 'grid')
def compare_grid(package, test_name, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    days = int(kwargs['days'])
    file_names = kwargs['file_name']
    ArtGrid(nightly_release, project, platform, nightly_tag).compare(package, test_name, days, file_names)


@dispatch.on('list', 'grid')
def list(package, job_type, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    ArtGrid(nightly_release, project, platform, nightly_tag).list(package, job_type)


@dispatch.on('log', 'grid')
def log(package, test_name, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    ArtGrid(nightly_release, project, platform, nightly_tag).log(package, test_name)


@dispatch.on('output', 'grid')
def output(package, test_name, file_name, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    ArtGrid(nightly_release, project, platform, nightly_tag).output(package, test_name, file_name)


@dispatch.on('retrieve')
def retrieve(job_id, **kwargs):
    """TBD."""
    url = 'http://bigpanda.cern.ch/task/' + job_id + '/?json'
    print 'Running art retrieve on ' + url
    sys.stdout.flush()

    response = requests.get(url)
    if response.status_code != 200:
        print 'ERROR http status code ' + str(response.status_code)
        sys.stdout.flush()
        exit(1)

    data = response.json()

    task = data['taskparams']['taskName']
    task_match = re.match(r'^(\w+)\.(\w+)\.(\w+)\.(\w+).([\w\.-]+)\.([\w-]+)\.([\w-]+)\.(\d+)/', task)
    if not task_match:
        print 'ERROR cannot decode task: ' + task
        sys.stdout.flush()
        exit(1)

    # job_type = task_match.group(1)
    # user = task_match.group(2)
    # experiment = task_match.group(3)
    # job = task_match.group(4)
    # nightly_release = task_match.group(5)
    # platform = task_match.group(6)
    # nightly_release = task_match.group(7)
    # build_id = task_match.group(8)

    status = data['task']['status']
    dsinfo = data['task']['dsinfo']
    print 'Total / Done / Failed:  ' + str(dsinfo['nfiles']) + ' / ' + str(dsinfo['nfilesfinished']) + ' / ' + str(dsinfo['nfilesfailed'])
    sys.stdout.flush()

    # Add other final states here
    print 'Status: ' + status
    sys.stdout.flush()

    if status in ['done']:
        exit(0)

    if status in ['finished', 'failed']:
        exit(1)

    if status in ['broken', 'aborted']:
        exit(2)

    # please re-call later
    exit(-1)


if __name__ == '__main__':
    dispatch(__doc__, version=os.path.splitext(os.path.basename(__file__))[0] + ' ' + __version__)
