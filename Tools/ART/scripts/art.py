#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART - ATLAS Release Tester.

Usage:
  art.py run             [-v --type=<T> --max-jobs=<N> --ci]  <script_directory> <sequence_tag>
  art.py grid            [-v --type=<T>]  <script_directory> <sequence_tag>
  art.py submit          [-v --type=<T>]  <sequence_tag> <nightly_release> <project> <platform> <nightly_tag>
  art.py validate        [-v]  <script_directory>
  art.py compare grid    [-v --days=<D>]  <nightly_release> <project> <platform> <nightly_tag> <package> <test_name> <file_name>...
  art.py compare ref     [-v]  <file_name> <ref_file>
  art.py list grid       [-v --json --type=<T>]  <package> <nightly_release> <project> <platform> <nightly_tag>
  art.py log grid        [-v]  <package> <test_name> <nightly_release> <project> <platform> <nightly_tag>
  art.py output grid     [-v]  <package> <test_name> <file_name> <nightly_release> <project> <platform> <nightly_tag>

Options:
  --ci              Run Continuous Integration tests only (using env: AtlasBuildBranch)
  --days=<D>        Number of days ago to pick up reference for compare [default: 1]
  --json            Output in json format
  --max-jobs=<N>    Maximum number of concurrent jobs to run
  --type=<T>        Type of job (e.g. grid, build)
  -h --help         Show this screen.
  -v, --verbose     Show details.
  --version         Show version.

Sub-commands:
  run               Run jobs from a package in a local build (needs release and grid setup)
  grid              Run jobs from a package on the grid (needs release and grid setup)
  submit            Submit nightly jobs to the grid (NOT for users)
  validate          Check headers in tests
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
  script_directory  Directory containing the package(s) with tests
  sequence_tag      Sequence tag (e.g. 0 or PIPELINE_ID)
  test_name         Name of the test inside the package (e.g. test_q322.sh)
  job_type          Type of job (e.g. grid, build)
"""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"
__version__ = '0.0.7'

import os
import sys

from ART.docopt_dispatch import dispatch

from ART import ArtBase, ArtGrid, ArtBuild


#
# First list the double commands
#

@dispatch.on('compare', 'ref')
def compare_ref(file_name, ref_file, **kwargs):
    """TBD."""
    exit(ArtBase().compare_ref(file_name, ref_file))


@dispatch.on('compare', 'grid')
def compare_grid(package, test_name, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    days = int(kwargs['days'])
    file_names = kwargs['file_name']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).compare(package, test_name, days, file_names))


@dispatch.on('list', 'grid')
def list(package, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    json_format = kwargs['json']
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    job_type = 'grid' if kwargs['type'] is None else kwargs['type']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).list(package, job_type, json_format))


@dispatch.on('log', 'grid')
def log(package, test_name, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).log(package, test_name))


@dispatch.on('output', 'grid')
def output(package, test_name, file_name, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).output(package, test_name, file_name))


@dispatch.on('submit')
def submit(sequence_tag, nightly_release, project, platform, nightly_tag, **kwargs):
    """TBD."""
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    type = 'grid' if kwargs['type'] is None else kwargs['type']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).task_list(type, sequence_tag))


@dispatch.on('grid')
def grid(script_directory, sequence_tag, **kwargs):
    """TBD."""
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    try:
        nightly_release = os.environ['AtlasBuildBranch']
        project = os.environ['AtlasProject']
        platform = os.environ[project + '_PLATFORM']
        nightly_tag = os.environ['AtlasBuildStamp']
    except KeyError, e:
        print "Environment variable not set", e
        sys.exit(1)
    type = 'grid' if kwargs['type'] is None else kwargs['type']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag, script_directory, True).task_list(type, sequence_tag))


@dispatch.on('run')
def run(script_directory, sequence_tag, **kwargs):
    """TBD."""
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    try:
        nightly_release = os.environ['AtlasBuildBranch']
        project = os.environ['AtlasProject']
        platform = os.environ[project + '_PLATFORM']
        nightly_tag = os.environ['AtlasBuildStamp']
    except KeyError, e:
        print "Environment variable not set", e
        sys.exit(1)
    type = 'build' if kwargs['type'] is None else kwargs['type']
    exit(ArtBuild(art_directory, nightly_release, project, platform, nightly_tag, script_directory, max_jobs=kwargs['max_jobs'], ci=kwargs['ci']).task_list(type, sequence_tag))


@dispatch.on('validate')
def validate(script_directory, **kwargs):
    """TBD."""
    exit(ArtBase().validate(script_directory))


if __name__ == '__main__':
    dispatch(__doc__, version=os.path.splitext(os.path.basename(__file__))[0] + ' ' + __version__)
