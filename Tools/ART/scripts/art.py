#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART - ATLAS Release Tester.

You need to setup for an ATLAS release before using ART.

Usage:
  art.py run             [-v -q --type=<T> --max-jobs=<N> --ci] <script_directory> <sequence_tag>
  art.py grid            [-v -q --type=<T> -n] <script_directory> <sequence_tag>
  art.py submit          [-v -q --type=<T> -n --config=<file>] <sequence_tag> [<package>]
  art.py copy            [-v -q --user=<user> --dst=<dir>] <package>
  art.py validate        [-v -q] <script_directory>
  art.py included        [-v -q --type=<T> --test-type=<TT>] <script_directory>
  art.py compare grid    [-v -q --days=<D> --user=<user> --entries=<entries>] <package> <test_name>
  art.py compare ref     [-v -q --entries=<entries>] <path> <ref_path>
  art.py download        [-v -q] <input_file>
  art.py list grid       [-v -q --user=<user> --json --type=<T> --test-type=<TT> --nogrid] <package>
  art.py log grid        [-v -q --user=<user>] <package> <test_name>
  art.py output grid     [-v -q --user=<user>] <package> <test_name>
  art.py config          [-v -q --config=<file>] <package>

Options:
  --ci                   Run Continuous Integration tests only (using env: AtlasBuildBranch)
  --config=<file>        Use specific config file [default: art-configuration.yml]
  --days=<D>             Number of days ago to pick up reference for compare [default: 1]
  --dst=<dir>            Destination directory for downloaded files
  --entries=<entries>    Number of entries to compare [default: 10]
  -h --help              Show this screen.
  --json                 Output in json format
  --max-jobs=<N>         Maximum number of concurrent jobs to run [default: 0]
  -n --no-action         No real submit will be done
  --nogrid               Do not retrieve grid indices
  -q --quiet             Show less information, only warnings and errors
  --test-type=<TT>       Type of test (e.g. all, batch or single) [default: all]
  --type=<T>             Type of job (e.g. grid, build)
  --user=<user>          User to use for RUCIO
  -v --verbose           Show more information, debug level
  --version              Show version.

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
  config            Show configuration

Arguments:
  input_file        Input file to download (e.g. CONTAINER_ID:ENTRY_NAME)
  package           Package of the test (e.g. Tier0ChainTests)
  path              Directory or File to compare
  ref_path          Directory or File to compare to
  script_directory  Directory containing the package(s) with tests
  sequence_tag      Sequence tag (e.g. 0 or PIPELINE_ID)
  test_name         Name of the test inside the package (e.g. test_q322.sh)

Environment:
  AtlasBuildBranch          Name of the nightly release (e.g. 21.0)
  AtlasProject              Name of the project (e.g. Athena)
  <AtlasProject>_PLATFORM   Platform (e.g. x86_64-slc6-gcc62-opt)
  AtlasBuildStamp           Nightly tag (e.g. 2017-02-26T2119)

Tests are called with:
  arguments: PACKAGE TEST_NAME SCRIPT_DIRECTORY TYPE [IN_FILE]
  environment: ArtScriptDirectory, ArtPackage, ArtJobType, ArtJobName, [ArtInFile]
"""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"
__version__ = '0.7.11'

import logging
import os
import sys

from ART.docopt_dispatch import dispatch

from ART import ArtBase, ArtGrid, ArtBuild

from ART.art_misc import get_atlas_env, set_log

MODULE = "art"

#
# First list the double commands
#


@dispatch.on('compare', 'ref')
def compare_ref(file_name, ref_file, **kwargs):
    """Compare the output of a job."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    entries = kwargs['entries']
    exit(ArtBase(art_directory).compare_ref(file_name, ref_file, entries))


@dispatch.on('compare', 'grid')
def compare_grid(package, test_name, **kwargs):
    """Compare the output of a job."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    days = int(kwargs['days'])
    entries = kwargs['entries']
    user = kwargs['user']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).compare(package, test_name, days, user, entries))


@dispatch.on('list', 'grid')
def list(package, **kwargs):
    """List the jobs of a package."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    job_type = 'grid' if kwargs['type'] is None else kwargs['type']
    index_type = kwargs['test_type']
    json_format = kwargs['json']
    user = kwargs['user']
    nogrid = kwargs['nogrid']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).list(package, job_type, index_type, json_format, user, nogrid))


@dispatch.on('log', 'grid')
def log(package, test_name, **kwargs):
    """Show the log of a job."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    user = kwargs['user']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).log(package, test_name, user))


@dispatch.on('output', 'grid')
def output(package, test_name, **kwargs):
    """Get the output of a job."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    user = kwargs['user']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).output(package, test_name, user))


@dispatch.on('submit')
def submit(sequence_tag, **kwargs):
    """Submit nightly jobs to the grid, NOT for users."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    job_type = 'grid' if kwargs['type'] is None else kwargs['type']
    package = kwargs['package']
    config = kwargs['config']
    no_action = kwargs['no_action']
    wait_and_copy = True
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag).task_list(job_type, sequence_tag, package, no_action, wait_and_copy, config))


@dispatch.on('grid')
def grid(script_directory, sequence_tag, **kwargs):
    """Run jobs from a package on the grid, needs release and grid setup."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    job_type = 'grid' if kwargs['type'] is None else kwargs['type']
    package = None
    no_action = kwargs['no_action']
    wait_and_copy = False
    config = None
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag, script_directory, True).task_list(job_type, sequence_tag, package, no_action, wait_and_copy, config))


@dispatch.on('run')
def run(script_directory, sequence_tag, **kwargs):
    """Run jobs from a package in a local build, needs release and grid setup."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    job_type = 'build' if kwargs['type'] is None else kwargs['type']
    exit(ArtBuild(art_directory, nightly_release, project, platform, nightly_tag, script_directory, max_jobs=int(kwargs['max_jobs']), ci=kwargs['ci']).task_list(job_type, sequence_tag))


@dispatch.on('copy')
def copy(package, **kwargs):
    """Copy outputs to eos area."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
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
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    art_type = 'grid' if kwargs['type'] is None else kwargs['type']
    index_type = kwargs['test_type']
    exit(ArtBase(art_directory).included(script_directory, art_type, index_type, nightly_release, project, platform))


@dispatch.on('config')
def config(package, **kwargs):
    """Show configuration."""
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    config = kwargs['config']
    exit(ArtBase(art_directory).config(package, nightly_release, project, platform, config))


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
