#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART-internal - ATLAS Release Tester (internal command).

Usage:
  art-internal.py build job   [-v -q]                 <script_directory> <sequence_tag> <package> <outfile> <job_type> <job_index>
  art-internal.py grid batch  [-v -q --skip-setup -n] <script_directory> <sequence_tag> <package> <outfile> <job_type> <job_index>
  art-internal.py grid single [-v -q --skip-setup --in=<in_file> -n] <script_directory> <sequence_tag> <package> <outfile> <job_name>

Options:
  -h --help         Show this screen.
  --skip-setup      Do not run atlas setup or voms
  --in=<in_file>    Normally percentage IN
  -n --no-action    No real submit will be done
  -q --quiet        Show less information, only warnings and errors
  -v --verbose      Show more information, debug level
  --version         Show version.

Arguments:
  job_index         Index of the test inside the package
  job_name          Index of the test (batch), or its name (single)
  job_type          Type of job (e.g. grid, ci, build)
  outfile           Tar filename used for the output of the job
  package           Package of the test (e.g. Tier0ChainTests)
  script_directory  Directory containing the package(s) with tests
  sequence_tag      Sequence tag (e.g. 0 or PIPELINE_ID)
  submit_directory  Temporary directory with all files for submission

Environment:
  AtlasBuildBranch          Name of the nightly release (e.g. 21.0)
  AtlasProject              Name of the project (e.g. Athena)
  <AtlasProject>_PLATFORM   Platform (e.g. x86_64-slc6-gcc62-opt)
  AtlasBuildStamp           Nightly tag (e.g. 2017-02-26T2119)
"""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import logging
import os
import sys

from ART.docopt_dispatch import dispatch

from ART import ArtGrid, ArtBuild

from ART.art_misc import get_atlas_env, set_log

MODULE = "art.internal"


@dispatch.on('build', 'job')
def build_job(script_directory, sequence_tag, package, outfile, job_type, job_index, **kwargs):
    """Build a single job, given a particular index.

    Tests are called with the following parameters:
    SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME
    """
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    exit(ArtBuild(art_directory, nightly_release, project, platform, nightly_tag, script_directory).job(sequence_tag, package, outfile, job_type, job_index))


@dispatch.on('grid', 'batch')
def grid_batch(script_directory, sequence_tag, package, outfile, job_type, job_index, **kwargs):
    """Run a batch job, given a particular index.

    Tests are called with the following parameters:
    SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME, STAGE
    """
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    skip_setup = kwargs['skip_setup']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag, script_directory, skip_setup).batch(sequence_tag, package, outfile, job_type, job_index))


@dispatch.on('grid', 'single')
def grid_single(script_directory, sequence_tag, package, outfile, job_name, **kwargs):
    """Run a single job, given a particular name.

    Tests are called with the following parameters:
    SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME, STAGE, IN_FILE
    """
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    (nightly_release, project, platform, nightly_tag) = get_atlas_env()
    skip_setup = kwargs['skip_setup']
    in_file = kwargs['in']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag, script_directory, skip_setup).single(sequence_tag, package, outfile, job_name, in_file))


if __name__ == '__main__':
    if sys.version_info < (2, 7, 0):
        sys.stderr.write("You need python 2.7 or later to run this script\n")
        exit(1)

    # NOTE: import should be here, to keep the order of the decorators (module first, art last and unused)
    from art import __version__
    logging.basicConfig()
    log = logging.getLogger(MODULE)
    log.setLevel(logging.INFO)
    log.info("ART_PATH %s", os.path.dirname(os.path.realpath(sys.argv[0])))
    dispatch(__doc__, version=os.path.splitext(os.path.basename(__file__))[0] + ' ' + __version__)
