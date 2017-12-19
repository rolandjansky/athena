#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART - ATLAS Release Tester.

Usage:
  art.py run             [-v -q --type=<T> --max-jobs=<N> --ci] <script_directory> <sequence_tag>
  art.py grid            [-v -q --type=<T> -n] <script_directory> <sequence_tag>
  art.py submit          [-v -q --type=<T> -n] <sequence_tag> <nightly_release> <project> <platform> <nightly_tag> [<package>]
  art.py copy            [-v -q --user=<user> --dst=<dir>] <nightly_release> <project> <platform> <nightly_tag> <package>
  art.py validate        [-v -q] <script_directory>
  art.py included        [-v -q --type=<T> --test-type=<TT>] <script_directory> [<nightly_release> <project> <platform>]
  art.py compare grid    [-v -q --days=<D> --user=<user>] <nightly_release> <project> <platform> <nightly_tag> <package> <test_name> <file_name>...
  art.py compare ref     [-v -q]  <file_name> <ref_file>
  art.py download        [-v -q] <input_file>
  art.py list grid       [-v -q --user=<user> --json --type=<T> --test-type=<TT> --nogrid] <package> <nightly_release> <project> <platform> <nightly_tag>
  art.py log grid        [-v -q --user=<user>] <package> <test_name> <nightly_release> <project> <platform> <nightly_tag>
  art.py output grid     [-v -q --user=<user>] <package> <test_name> <nightly_release> <project> <platform> <nightly_tag>

Options:
  --ci              Run Continuous Integration tests only (using env: AtlasBuildBranch)
  --days=<D>        Number of days ago to pick up reference for compare [default: 1]
  --dst=<dir>       Destination directory for downloaded files
  -h --help         Show this screen.
  --json            Output in json format
  --max-jobs=<N>    Maximum number of concurrent jobs to run [default: 0]
  -n --no-action    No real submit will be done
  --nogrid          Do not retrieve grid indices
  -q --quiet        Show less information, only warnings and errors
  --test-type=<TT>  Type of test (e.g. all, batch or single) [default: all]
  --type=<T>        Type of job (e.g. grid, build)
  --user=<user>     User to use for RUCIO
  -v --verbose      Show more information, debug level
  --version         Show version.

Sub-commands:
  run               Run jobs from a package in a local build (needs release and grid setup)
  grid              Run jobs from a package on the grid (needs release and grid setup)
  submit            Submit nightly jobs to the grid (NOT for users)
  copy              Copy outputs and logs from RUCIO
  validate          Check headers in tests
  included          Show list of files which will be included for art submit/art grid
  compare           Compare the output of a job
  download          Download a file from rucio
  list              List the jobs of a package
  log               Show the log of a job
  output            Get the output of a job

Arguments:
  file_name         Filename to save the output to
  index             Index of the test inside the package
  input_file        Input file to download (e.g. CONTAINER_ID:ENTRY_NAME)
  nightly_release   Name of the nightly release (e.g. 21.0)
  nightly_tag       Nightly tag (e.g. 2017-02-26T2119)
  out               Tar filename used for the output of the job
  package           Package of the test (e.g. Tier0ChainTests)
  platform          Platform (e.g. x86_64-slc6-gcc62-opt)
  project           Name of the project (e.g. Athena)
  script_directory  Directory containing the package(s) with tests
  sequence_tag      Sequence tag (e.g. 0 or PIPELINE_ID)
  test_name         Name of the test inside the package (e.g. test_q322.sh)
"""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"
__version__ = '0.6.10'

import logging
import os
import sys

from ART.docopt_dispatch import dispatch

from ART import ArtBase, ArtGrid, ArtBuild

from ART.art_misc import set_log

MODULE = "art"

#
# First list the double commands
#


@dispatch.on('compare', 'ref')
def compare_ref(file_name, ref_file, **kwargs):
    """Compare the output of a job."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    exit(ArtBase(art_directory).compare_ref(file_name, ref_file))


@dispatch.on('compare', 'grid')
def compare_grid(package, test_name, nightly_release, project, platform, nightly_tag, **kwargs):
    """Compare the output of a job."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    days = int(kwargs['days'])
    file_names = kwargs['file_name']
    user = kwargs['user']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).compare(package, test_name, days, file_names, user))


@dispatch.on('list', 'grid')
def list(package, nightly_release, project, platform, nightly_tag, **kwargs):
    """List the jobs of a package."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    job_type = 'grid' if kwargs['type'] is None else kwargs['type']
    index_type = kwargs['test_type']
    json_format = kwargs['json']
    user = kwargs['user']
    nogrid = kwargs['nogrid']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).list(package, job_type, index_type, json_format, user, nogrid))


@dispatch.on('log', 'grid')
def log(package, test_name, nightly_release, project, platform, nightly_tag, **kwargs):
    """Show the log of a job."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    user = kwargs['user']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).log(package, test_name, user))


@dispatch.on('output', 'grid')
def output(package, test_name, nightly_release, project, platform, nightly_tag, **kwargs):
    """Get the output of a job."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    user = kwargs['user']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).output(package, test_name, user))


@dispatch.on('submit')
def submit(sequence_tag, nightly_release, project, platform, nightly_tag, **kwargs):
    """Submit nightly jobs to the grid, NOT for users."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    job_type = 'grid' if kwargs['type'] is None else kwargs['type']
    package = kwargs['package']
    no_action = kwargs['no_action']
    wait_and_copy = True
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).task_list(job_type, sequence_tag, package, no_action, wait_and_copy))


@dispatch.on('grid')
def grid(script_directory, sequence_tag, **kwargs):
    """Run jobs from a package on the grid, needs release and grid setup."""
    set_log(kwargs)
    log = logging.getLogger(MODULE)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    try:
        nightly_release = os.environ['AtlasBuildBranch']
        project = os.environ['AtlasProject']
        platform = os.environ[project + '_PLATFORM']
        nightly_tag = os.environ['AtlasBuildStamp']
    except KeyError, e:
        log.critical("Environment variable not set %s", e)
        sys.exit(1)
    job_type = 'grid' if kwargs['type'] is None else kwargs['type']
    package = None
    no_action = kwargs['no_action']
    wait_and_copy = False
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag, script_directory, True).task_list(job_type, sequence_tag, package, no_action, wait_and_copy))


@dispatch.on('run')
def run(script_directory, sequence_tag, **kwargs):
    """Run jobs from a package in a local build, needs release and grid setup."""
    set_log(kwargs)
    log = logging.getLogger(MODULE)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    try:
        nightly_release = os.environ['AtlasBuildBranch']
        project = os.environ['AtlasProject']
        platform = os.environ[project + '_PLATFORM']
        nightly_tag = os.environ['AtlasBuildStamp']
    except KeyError, e:
        log.critical("Environment variable not set %s", e)
        sys.exit(1)
    job_type = 'build' if kwargs['type'] is None else kwargs['type']
    exit(ArtBuild(art_directory, nightly_release, project, platform, nightly_tag, script_directory, max_jobs=int(kwargs['max_jobs']), ci=kwargs['ci']).task_list(job_type, sequence_tag))


@dispatch.on('copy')
def copy(nightly_release, project, platform, nightly_tag, **kwargs):
    """Copy outputs to eos area."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    package = kwargs['package']
    # NOTE: default depends on USER, not set it here but in ArtGrid.copy
    dst = kwargs['dst']
    user = kwargs['user']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).copy(package, dst, user))


@dispatch.on('validate')
def validate(script_directory, **kwargs):
    """Check headers in tests."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    exit(ArtBase(art_directory).validate(script_directory))


@dispatch.on('included')
def included(script_directory, **kwargs):
    """Show list of files which will be included for art submit/art grid."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    nightly_release = os.environ['AtlasBuildBranch'] if kwargs['nightly_release'] is None else kwargs['nightly_release']
    project = os.environ['AtlasProject'] if kwargs['project'] is None else kwargs['project']
    platform = os.environ[project + '_PLATFORM'] if kwargs['platform'] is None else kwargs['platform']
    art_type = 'grid' if kwargs['type'] is None else kwargs['type']
    index_type = kwargs['test_type']
    exit(ArtBase(art_directory).included(script_directory, art_type, index_type, nightly_release, project, platform))


@dispatch.on('download')
def download(input_file, **kwargs):
    """Download a file from rucio."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    exit(ArtBase(art_directory).download(input_file))


if __name__ == '__main__':
    if sys.version_info < (2, 7, 0):
        sys.stderr.write("You need python 2.7 or later to run this script\n")
        exit(1)

    logging.basicConfig()
    dispatch(__doc__, version=os.path.splitext(os.path.basename(__file__))[0] + ' ' + __version__)
