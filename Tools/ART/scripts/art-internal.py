#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART-internal - ATLAS Release Tester (internal command).

Usage:
  art-internal.py job build   [-v -q]  <script_directory> <package> <job_type> <sequence_tag> <index> <out> <nightly_release> <project> <platform> <nightly_tag>
  art-internal.py job grid    [-v -q --skip-setup]  <script_directory> <package> <job_type> <sequence_tag> <index_type> <index_or_name> <out> <nightly_release> <project> <platform> <nightly_tag>

Options:
  --skip-setup      Do not run atlas setup or voms
  -h --help         Show this screen.
  -q --quiet        Show less information, only warnings and errors
  -v --verbose      Show more information, debug level
  --version         Show version.

Sub-commands:
  job               Run a single job, given a particular index
  copy              Copy outputs to eos area

Arguments:
  index_type        Type of index used (e.g. batch or single)
  index             Index of the test inside the package
  index_or_name     Index of the test (batch), or its name (single)
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

import logging
import os
import sys

from ART.docopt_dispatch import dispatch

from ART import ArtGrid, ArtBuild

from ART.art_misc import set_log

MODULE = "art.internal"


@dispatch.on('job', 'build')
def job_build(script_directory, package, job_type, sequence_tag, index, out, nightly_release, project, platform, nightly_tag, **kwargs):
    """Run a single job, given a particular index.

    Tests are called with the following parameters:
    SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME
    """
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    exit(ArtBuild(art_directory, nightly_release, project, platform, nightly_tag, script_directory).job(package, job_type, sequence_tag, index, out))


@dispatch.on('job', 'grid')
def job_grid(script_directory, package, job_type, sequence_tag, index_type, index_or_name, out, nightly_release, project, platform, nightly_tag, **kwargs):
    """Run a single job, given a particular index.

    Tests are called with the following parameters:
    SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME, NIGHTLY_RELEASE, PROJECT, PLATFORM, NIGHTLY_TAG
    """
    set_log(kwargs)
    art_directory = os.path.dirname(os.path.realpath(sys.argv[0]))
    skip_setup = kwargs['skip_setup']
    exit(ArtGrid(art_directory, nightly_release, project, platform, nightly_tag, script_directory, skip_setup).job(package, job_type, sequence_tag, index_type, index_or_name, out))


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
